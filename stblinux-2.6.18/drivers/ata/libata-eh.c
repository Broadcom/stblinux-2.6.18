/*
 *  libata-eh.c - libata error handling
 *
 *  Maintained by:  Jeff Garzik <jgarzik@pobox.com>
 *    		    Please ALWAYS copy linux-ide@vger.kernel.org
 *		    on emails.
 *
 *  Copyright 2006 Tejun Heo <htejun@gmail.com>
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139,
 *  USA.
 *
 *
 *  libata documentation is available via 'make {ps|pdf}docs',
 *  as Documentation/DocBook/libata.*
 *
 *  Hardware documentation available from http://www.t13.org/ and
 *  http://www.sata-io.org/
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <scsi/scsi.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_eh.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_cmnd.h>
#include "../scsi/scsi_transport_api.h"

#include <linux/libata.h>

#include "libata.h"

static unsigned long hotplug_polling_interval = 2000;
module_param(hotplug_polling_interval, ulong, 0644);
MODULE_PARM_DESC(hotplug_polling_interval,
		 "Hotplug polling interval in milliseconds (default 2000)");

static void __ata_port_freeze(struct ata_port *ap);
static void ata_eh_handle_port_suspend(struct ata_port *ap);
static void ata_eh_handle_port_resume(struct ata_port *ap);

#if defined (CONFIG_MIPS_BCM_NDVD)
static unsigned int atapi_eh_request_sense(struct ata_device *dev, unsigned char *sense_buf);
#endif

static void ata_ering_record(struct ata_ering *ering, int is_io,
			     unsigned int err_mask)
{
	struct ata_ering_entry *ent;

	WARN_ON(!err_mask);

	ering->cursor++;
	ering->cursor %= ATA_ERING_SIZE;

	ent = &ering->ring[ering->cursor];
	ent->is_io = is_io;
	ent->err_mask = err_mask;
	ent->timestamp = get_jiffies_64();
}

static struct ata_ering_entry * ata_ering_top(struct ata_ering *ering)
{
	struct ata_ering_entry *ent = &ering->ring[ering->cursor];
	if (!ent->err_mask)
		return NULL;
	return ent;
}

static int ata_ering_map(struct ata_ering *ering,
			 int (*map_fn)(struct ata_ering_entry *, void *),
			 void *arg)
{
	int idx, rc = 0;
	struct ata_ering_entry *ent;

	idx = ering->cursor;
	do {
		ent = &ering->ring[idx];
		if (!ent->err_mask)
			break;
		rc = map_fn(ent, arg);
		if (rc)
			break;
		idx = (idx - 1 + ATA_ERING_SIZE) % ATA_ERING_SIZE;
	} while (idx != ering->cursor);

	return rc;
}

static unsigned int ata_eh_dev_action(struct ata_device *dev)
{
	struct ata_eh_context *ehc = &dev->link->eh_context;

	return ehc->i.action | ehc->i.dev_action[dev->devno];
}

static void ata_eh_clear_action(struct ata_link *link, struct ata_device *dev,
				struct ata_eh_info *ehi, unsigned int action)
{
	struct ata_device *tdev;

	if (!dev) {
		ehi->action &= ~action;
		ata_link_for_each_dev(tdev, link)
			ehi->dev_action[tdev->devno] &= ~action;
	} else {
		/* doesn't make sense for port-wide EH actions */
		WARN_ON(!(action & ATA_EH_PERDEV_MASK));

		/* break ehi->action into ehi->dev_action */
		if (ehi->action & action) {
			ata_link_for_each_dev(tdev, link)
				ehi->dev_action[tdev->devno] |=
					ehi->action & action;
			ehi->action &= ~action;
		}

		/* turn off the specified per-dev action */
		ehi->dev_action[dev->devno] &= ~action;
	}
}

/**
 *	ata_scsi_timed_out - SCSI layer time out callback
 *	@cmd: timed out SCSI command
 *
 *	Handles SCSI layer timeout.  We race with normal completion of
 *	the qc for @cmd.  If the qc is already gone, we lose and let
 *	the scsi command finish (EH_HANDLED).  Otherwise, the qc has
 *	timed out and EH should be invoked.  Prevent ata_qc_complete()
 *	from finishing it by setting EH_SCHEDULED and return
 *	EH_NOT_HANDLED.
 *
 *	TODO: kill this function once old EH is gone.
 *
 *	LOCKING:
 *	Called from timer context
 *
 *	RETURNS:
 *	EH_HANDLED or EH_NOT_HANDLED
 */
enum scsi_eh_timer_return ata_scsi_timed_out(struct scsi_cmnd *cmd)
{
	struct Scsi_Host *host = cmd->device->host;
	struct ata_port *ap = ata_shost_to_port(host);
	unsigned long flags;
	struct ata_queued_cmd *qc;
	enum scsi_eh_timer_return ret;

	DPRINTK("ENTER\n");

	if (ap->ops->error_handler) {
		ret = EH_NOT_HANDLED;
		goto out;
	}

	ret = EH_HANDLED;
	spin_lock_irqsave(ap->lock, flags);
	qc = ata_qc_from_tag(ap, ap->link.active_tag);
	if (qc) {
		WARN_ON(qc->scsicmd != cmd);
		qc->flags |= ATA_QCFLAG_EH_SCHEDULED;
		qc->err_mask |= AC_ERR_TIMEOUT;
		ret = EH_NOT_HANDLED;
	}
	spin_unlock_irqrestore(ap->lock, flags);

 out:
	DPRINTK("EXIT, ret=%d\n", ret);
	return ret;
}

/**
 *	ata_scsi_error - SCSI layer error handler callback
 *	@host: SCSI host on which error occurred
 *
 *	Handles SCSI-layer-thrown error events.
 *
 *	LOCKING:
 *	Inherited from SCSI layer (none, can sleep)
 *
 *	RETURNS:
 *	Zero.
 */
void ata_scsi_error(struct Scsi_Host *host)
{
	struct ata_port *ap = ata_shost_to_port(host);
	int i, repeat_cnt = ATA_EH_MAX_REPEAT;
	unsigned long flags;
#if defined (CONFIG_MIPS_BCM_NDVD)
	u8 stat;
	int issue_request_sense = 0;
	int sns;
	struct ata_queued_cmd *sns_qc = NULL;
#endif

	DPRINTK("ENTER\n");

	/* synchronize with port task */
	ata_port_flush_task(ap);

	/* synchronize with host lock and sort out timeouts */

	/* For new EH, all qcs are finished in one of three ways -
	 * normal completion, error completion, and SCSI timeout.
	 * Both cmpletions can race against SCSI timeout.  When normal
	 * completion wins, the qc never reaches EH.  When error
	 * completion wins, the qc has ATA_QCFLAG_FAILED set.
	 *
	 * When SCSI timeout wins, things are a bit more complex.
	 * Normal or error completion can occur after the timeout but
	 * before this point.  In such cases, both types of
	 * completions are honored.  A scmd is determined to have
	 * timed out iff its associated qc is active and not failed.
	 */
	if (ap->ops->error_handler) {
		struct scsi_cmnd *scmd, *tmp;
		int nr_timedout = 0;

		spin_lock_irqsave(ap->lock, flags);

		list_for_each_entry_safe(scmd, tmp, &host->eh_cmd_q, eh_entry) {
			struct ata_queued_cmd *qc;

			for (i = 0; i < ATA_MAX_QUEUE; i++) {
				qc = __ata_qc_from_tag(ap, i);
				if (qc->flags & ATA_QCFLAG_ACTIVE &&
				    qc->scsicmd == scmd)
					break;
			}

			if (i < ATA_MAX_QUEUE) {
				/* the scmd has an associated qc */
				if (!(qc->flags & ATA_QCFLAG_FAILED)) {
#if defined (CONFIG_MIPS_BCM_NDVD)
					stat  = ata_altstatus(ap);
					if ((stat & (ATA_BUSY | ATA_DRDY | ATA_ERR)) == (ATA_DRDY | ATA_ERR)) {
						issue_request_sense = 1;
						sns_qc = qc;

						/*
						 * Make sure that the DMA engine is stopped
						 * and interrupts cleared so that we can
						 * send the Request Sense below.
						 */
						ap->ops->bmdma_stop(qc);
						ata_chk_status(ap);
						ap->ops->irq_clear(ap);
					}
					else

#endif
					{
					/* which hasn't failed yet, timeout */
					qc->err_mask |= AC_ERR_TIMEOUT;
					qc->flags |= ATA_QCFLAG_FAILED;
					nr_timedout++;
				}
				}
			} else {
				/* Normal completion occurred after
				 * SCSI timeout but before this point.
				 * Successfully complete it.
				 */
				scmd->retries = scmd->allowed;
				scsi_eh_finish_cmd(scmd, &ap->eh_done_q);
			}
		}

		/* If we have timed out qcs.  They belong to EH from
		 * this point but the state of the controller is
		 * unknown.  Freeze the port to make sure the IRQ
		 * handler doesn't diddle with those qcs.  This must
		 * be done atomically w.r.t. setting QCFLAG_FAILED.
		 */
		if (nr_timedout)
			__ata_port_freeze(ap);

		spin_unlock_irqrestore(ap->lock, flags);
	} else
		spin_unlock_wait(ap->lock);

#if defined (CONFIG_MIPS_BCM_NDVD)
	if (issue_request_sense && sns_qc) {
		sns = atapi_eh_request_sense(sns_qc->dev, sns_qc->scsicmd->sense_buffer);
		if (!sns) {
			/* ATA_QCFLAG_SENSE_VALID is used to
			 * tell atapi_qc_complete() that sense
			 * data is already valid.
			 *
			 * TODO: interpret sense data and set
			 * appropriate err_mask.
			 */
			sns_qc->flags |= ATA_QCFLAG_SENSE_VALID;

			/*
			 * Successfully complete it.
			 */
			ata_qc_complete(sns_qc);
			return;
		}
	}
#endif

 repeat:
	/* invoke error handler */
	if (ap->ops->error_handler) {
		struct ata_link *link;

		/* process port resume request */
		ata_eh_handle_port_resume(ap);

		/* fetch & clear EH info */
		spin_lock_irqsave(ap->lock, flags);

		__ata_port_for_each_link(link, ap) {
			memset(&link->eh_context, 0, sizeof(link->eh_context));
			link->eh_context.i = link->eh_info;
			memset(&link->eh_info, 0, sizeof(link->eh_info));
		}

		ap->pflags |= ATA_PFLAG_EH_IN_PROGRESS;
		ap->pflags &= ~ATA_PFLAG_EH_PENDING;
		ap->excl_link = NULL;	/* don't maintain exclusion over EH */

		spin_unlock_irqrestore(ap->lock, flags);

		/* invoke EH, skip if unloading or suspended */
		if (!(ap->pflags & (ATA_PFLAG_UNLOADING | ATA_PFLAG_SUSPENDED)))
			ap->ops->error_handler(ap);
		else
			ata_eh_finish(ap);

		/* process port suspend request */
		ata_eh_handle_port_suspend(ap);

		/* Exception might have happend after ->error_handler
		 * recovered the port but before this point.  Repeat
		 * EH in such case.
		 */
		spin_lock_irqsave(ap->lock, flags);

		if (ap->pflags & ATA_PFLAG_EH_PENDING) {
			if (--repeat_cnt) {
				ata_port_printk(ap, KERN_INFO,
					"EH pending after completion, "
					"repeating EH (cnt=%d)\n", repeat_cnt);
				spin_unlock_irqrestore(ap->lock, flags);
				goto repeat;
			}
			ata_port_printk(ap, KERN_ERR, "EH pending after %d "
					"tries, giving up\n", ATA_EH_MAX_REPEAT);
		}

		/* this run is complete, make sure EH info is clear */
		__ata_port_for_each_link(link, ap)
			memset(&link->eh_info, 0, sizeof(link->eh_info));

		/* Clear host_eh_scheduled while holding ap->lock such
		 * that if exception occurs after this point but
		 * before EH completion, SCSI midlayer will
		 * re-initiate EH.
		 */
		host->host_eh_scheduled = 0;

		spin_unlock_irqrestore(ap->lock, flags);
	} else {
		WARN_ON(ata_qc_from_tag(ap, ap->link.active_tag) == NULL);
		ap->ops->eng_timeout(ap);
	}

	/* finish or retry handled scmd's and clean up */
	WARN_ON(host->host_failed || !list_empty(&host->eh_cmd_q));

	scsi_eh_flush_done_q(&ap->eh_done_q);

	/* clean up */
	spin_lock_irqsave(ap->lock, flags);

	if (ap->pflags & ATA_PFLAG_LOADING)
		ap->pflags &= ~ATA_PFLAG_LOADING;
	else if (ap->pflags & ATA_PFLAG_SCSI_HOTPLUG)
		queue_work(ata_aux_wq, &ap->hotplug_task);

	if (ap->pflags & ATA_PFLAG_RECOVERED)
		ata_port_printk(ap, KERN_INFO, "EH complete\n");

	ap->pflags &= ~(ATA_PFLAG_SCSI_HOTPLUG | ATA_PFLAG_RECOVERED);

	/* tell wait_eh that we're done */
	ap->pflags &= ~ATA_PFLAG_EH_IN_PROGRESS;
	wake_up_all(&ap->eh_wait_q);

	spin_unlock_irqrestore(ap->lock, flags);

	DPRINTK("EXIT\n");
}

/**
 *	ata_port_wait_eh - Wait for the currently pending EH to complete
 *	@ap: Port to wait EH for
 *
 *	Wait until the currently pending EH is complete.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 */
void ata_port_wait_eh(struct ata_port *ap)
{
	unsigned long flags;
	DEFINE_WAIT(wait);

 retry:
	spin_lock_irqsave(ap->lock, flags);

	while (ap->pflags & (ATA_PFLAG_EH_PENDING | ATA_PFLAG_EH_IN_PROGRESS)) {
		prepare_to_wait(&ap->eh_wait_q, &wait, TASK_UNINTERRUPTIBLE);
		spin_unlock_irqrestore(ap->lock, flags);
		schedule();
		spin_lock_irqsave(ap->lock, flags);
	}
	finish_wait(&ap->eh_wait_q, &wait);

	spin_unlock_irqrestore(ap->lock, flags);

	/* make sure SCSI EH is complete */
	if (scsi_host_in_recovery(ap->scsi_host)) {
		msleep(10);
		goto retry;
	}
}

/**
 *	ata_qc_timeout - Handle timeout of queued command
 *	@qc: Command that timed out
 *
 *	Some part of the kernel (currently, only the SCSI layer)
 *	has noticed that the active command on port @ap has not
 *	completed after a specified length of time.  Handle this
 *	condition by disabling DMA (if necessary) and completing
 *	transactions, with error if necessary.
 *
 *	This also handles the case of the "lost interrupt", where
 *	for some reason (possibly hardware bug, possibly driver bug)
 *	an interrupt was not delivered to the driver, even though the
 *	transaction completed successfully.
 *
 *	TODO: kill this function once old EH is gone.
 *
 *	LOCKING:
 *	Inherited from SCSI layer (none, can sleep)
 */
static void ata_qc_timeout(struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;
	u8 host_stat = 0, drv_stat;
	unsigned long flags;

	DPRINTK("ENTER\n");

	ap->hsm_task_state = HSM_ST_IDLE;

	spin_lock_irqsave(ap->lock, flags);

	switch (qc->tf.protocol) {

	case ATA_PROT_DMA:
	case ATA_PROT_ATAPI_DMA:
		host_stat = ap->ops->bmdma_status(ap);

		/* before we do anything else, clear DMA-Start bit */
		ap->ops->bmdma_stop(qc);

		/* fall through */

	default:
		ata_altstatus(ap);
		drv_stat = ata_chk_status(ap);

		/* ack bmdma irq events */
		ap->ops->irq_clear(ap);

		ata_dev_printk(qc->dev, KERN_ERR, "command 0x%x timeout, "
			       "stat 0x%x host_stat 0x%x\n",
			       qc->tf.command, drv_stat, host_stat);

		/* complete taskfile transaction */
		qc->err_mask |= AC_ERR_TIMEOUT;
		break;
	}

	spin_unlock_irqrestore(ap->lock, flags);

	ata_eh_qc_complete(qc);

	DPRINTK("EXIT\n");
}

/**
 *	ata_eng_timeout - Handle timeout of queued command
 *	@ap: Port on which timed-out command is active
 *
 *	Some part of the kernel (currently, only the SCSI layer)
 *	has noticed that the active command on port @ap has not
 *	completed after a specified length of time.  Handle this
 *	condition by disabling DMA (if necessary) and completing
 *	transactions, with error if necessary.
 *
 *	This also handles the case of the "lost interrupt", where
 *	for some reason (possibly hardware bug, possibly driver bug)
 *	an interrupt was not delivered to the driver, even though the
 *	transaction completed successfully.
 *
 *	TODO: kill this function once old EH is gone.
 *
 *	LOCKING:
 *	Inherited from SCSI layer (none, can sleep)
 */
void ata_eng_timeout(struct ata_port *ap)
{
	DPRINTK("ENTER\n");

	ata_qc_timeout(ata_qc_from_tag(ap, ap->link.active_tag));

	DPRINTK("EXIT\n");
}

/**
 *	ata_qc_schedule_eh - schedule qc for error handling
 *	@qc: command to schedule error handling for
 *
 *	Schedule error handling for @qc.  EH will kick in as soon as
 *	other commands are drained.
 *
 *	LOCKING:
 *	spin_lock_irqsave(host lock)
 */
void ata_qc_schedule_eh(struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;

	WARN_ON(!ap->ops->error_handler);

	qc->flags |= ATA_QCFLAG_FAILED;
	qc->ap->pflags |= ATA_PFLAG_EH_PENDING;

	/* The following will fail if timeout has already expired.
	 * ata_scsi_error() takes care of such scmds on EH entry.
	 * Note that ATA_QCFLAG_FAILED is unconditionally set after
	 * this function completes.
	 */
	scsi_req_abort_cmd(qc->scsicmd);
}

/**
 *	ata_port_schedule_eh - schedule error handling without a qc
 *	@ap: ATA port to schedule EH for
 *
 *	Schedule error handling for @ap.  EH will kick in as soon as
 *	all commands are drained.
 *
 *	LOCKING:
 *	spin_lock_irqsave(host lock)
 */
void ata_port_schedule_eh(struct ata_port *ap)
{
	WARN_ON(!ap->ops->error_handler);

	ap->pflags |= ATA_PFLAG_EH_PENDING;
	scsi_schedule_eh(ap->scsi_host);

	DPRINTK("port EH scheduled\n");
}

static int ata_do_link_abort(struct ata_port *ap, struct ata_link *link)
{
	int tag, nr_aborted = 0;

	WARN_ON(!ap->ops->error_handler);

	for (tag = 0; tag < ATA_MAX_QUEUE; tag++) {
		struct ata_queued_cmd *qc = ata_qc_from_tag(ap, tag);

		if (qc && (!link || qc->dev->link == link)) {
			qc->flags |= ATA_QCFLAG_FAILED;
			ata_qc_complete(qc);
			nr_aborted++;
		}
	}

	if (!nr_aborted)
		ata_port_schedule_eh(ap);

	return nr_aborted;
}

/**
 *	ata_link_abort - abort all qc's on the link
 *	@link: ATA link to abort qc's for
 *
 *	Abort all active qc's active on @link and schedule EH.
 *
 *	LOCKING:
 *	spin_lock_irqsave(host lock)
 *
 *	RETURNS:
 *	Number of aborted qc's.
 */
int ata_link_abort(struct ata_link *link)
{
	return ata_do_link_abort(link->ap, link);
}

/**
 *	ata_port_abort - abort all qc's on the port
 *	@ap: ATA port to abort qc's for
 *
 *	Abort all active qc's of @ap and schedule EH.
 *
 *	LOCKING:
 *	spin_lock_irqsave(host_set lock)
 *
 *	RETURNS:
 *	Number of aborted qc's.
 */
int ata_port_abort(struct ata_port *ap)
{
	return ata_do_link_abort(ap, NULL);
}

/**
 *	__ata_port_freeze - freeze port
 *	@ap: ATA port to freeze
 *
 *	This function is called when HSM violation or some other
 *	condition disrupts normal operation of the port.  Frozen port
 *	is not allowed to perform any operation until the port is
 *	thawed, which usually follows a successful reset.
 *
 *	ap->ops->freeze() callback can be used for freezing the port
 *	hardware-wise (e.g. mask interrupt and stop DMA engine).  If a
 *	port cannot be frozen hardware-wise, the interrupt handler
 *	must ack and clear interrupts unconditionally while the port
 *	is frozen.
 *
 *	LOCKING:
 *	spin_lock_irqsave(host lock)
 */
static void __ata_port_freeze(struct ata_port *ap)
{
	WARN_ON(!ap->ops->error_handler);

	if (ap->ops->freeze)
		ap->ops->freeze(ap);

	ap->pflags |= ATA_PFLAG_FROZEN;

	DPRINTK("ata%u port frozen\n", ap->id);
}

/**
 *	ata_port_freeze - abort & freeze port
 *	@ap: ATA port to freeze
 *
 *	Abort and freeze @ap.
 *
 *	LOCKING:
 *	spin_lock_irqsave(host lock)
 *
 *	RETURNS:
 *	Number of aborted commands.
 */
int ata_port_freeze(struct ata_port *ap)
{
	int nr_aborted;

	WARN_ON(!ap->ops->error_handler);

	nr_aborted = ata_port_abort(ap);
	__ata_port_freeze(ap);

	return nr_aborted;
}

/**
 *	ata_eh_freeze_port - EH helper to freeze port
 *	@ap: ATA port to freeze
 *
 *	Freeze @ap.  As the 'freeze' operation means 'shutdown event
 *	reporting', it is a perfect place to deactivate hp-poll.  Note
 *	that ata_port_freeze() always invokes EH and eventually this
 *	function, so deactivating hp-poll in this function is enough.
 *
 *	LOCKING:
 *	None.
 */
void ata_eh_freeze_port(struct ata_port *ap)
{
	unsigned long flags;

	if (!ap->ops->error_handler)
		return;

	ata_hp_poll_deactivate(ap);

	spin_lock_irqsave(ap->lock, flags);
	__ata_port_freeze(ap);
	spin_unlock_irqrestore(ap->lock, flags);
}

/**
 *	ata_port_thaw_port - EH helper to thaw port
 *	@ap: ATA port to thaw
 *
 *	Thaw frozen port @ap and activate hp-poll if necessary.
 *
 *	LOCKING:
 *	None.
 */
void ata_eh_thaw_port(struct ata_port *ap)
{
	unsigned long flags;

	if (!ap->ops->error_handler)
		return;

	spin_lock_irqsave(ap->lock, flags);

	ap->pflags &= ~ATA_PFLAG_FROZEN;

	if (ap->ops->thaw)
		ap->ops->thaw(ap);

	spin_unlock_irqrestore(ap->lock, flags);

	if (ap->flags & ATA_FLAG_HP_POLLING)
		ata_hp_poll_activate(ap);

	DPRINTK("ata%u port thawed\n", ap->id);
}

static void ata_eh_scsidone(struct scsi_cmnd *scmd)
{
	/* nada */
}

static void __ata_eh_qc_complete(struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;
	struct scsi_cmnd *scmd = qc->scsicmd;
	unsigned long flags;

	spin_lock_irqsave(ap->lock, flags);
	qc->scsidone = ata_eh_scsidone;
	__ata_qc_complete(qc);
	WARN_ON(ata_tag_valid(qc->tag));
	spin_unlock_irqrestore(ap->lock, flags);

	scsi_eh_finish_cmd(scmd, &ap->eh_done_q);
}

/**
 *	ata_eh_qc_complete - Complete an active ATA command from EH
 *	@qc: Command to complete
 *
 *	Indicate to the mid and upper layers that an ATA command has
 *	completed.  To be used from EH.
 */
void ata_eh_qc_complete(struct ata_queued_cmd *qc)
{
	struct scsi_cmnd *scmd = qc->scsicmd;
	scmd->retries = scmd->allowed;
	__ata_eh_qc_complete(qc);
}

/**
 *	ata_eh_qc_retry - Tell midlayer to retry an ATA command after EH
 *	@qc: Command to retry
 *
 *	Indicate to the mid and upper layers that an ATA command
 *	should be retried.  To be used from EH.
 *
 *	SCSI midlayer limits the number of retries to scmd->allowed.
 *	scmd->retries is decremented for commands which get retried
 *	due to unrelated failures (qc->err_mask is zero).
 */
void ata_eh_qc_retry(struct ata_queued_cmd *qc)
{
	struct scsi_cmnd *scmd = qc->scsicmd;
	if (!qc->err_mask && scmd->retries)
		scmd->retries--;
	__ata_eh_qc_complete(qc);
}

/**
 *	ata_eh_detach_dev - detach ATA device
 *	@dev: ATA device to detach
 *
 *	Detach @dev.
 *
 *	LOCKING:
 *	None.
 */
void ata_eh_detach_dev(struct ata_device *dev)
{
	struct ata_link *link = dev->link;
	struct ata_port *ap = link->ap;
	unsigned long flags;

	ata_dev_disable(dev);

	spin_lock_irqsave(ap->lock, flags);

	dev->flags &= ~ATA_DFLAG_DETACH;

	if (ata_scsi_offline_dev(dev)) {
		dev->flags |= ATA_DFLAG_DETACHED;
		ap->pflags |= ATA_PFLAG_SCSI_HOTPLUG;
	}

	/* clear per-dev EH actions */
	ata_eh_clear_action(link, dev, &link->eh_info, ATA_EH_PERDEV_MASK);
	ata_eh_clear_action(link, dev, &link->eh_context.i, ATA_EH_PERDEV_MASK);

	spin_unlock_irqrestore(ap->lock, flags);
}

/**
 *	ata_eh_about_to_do - about to perform eh_action
 *	@link: target ATA link
 *	@dev: target ATA dev for per-dev action (can be NULL)
 *	@action: action about to be performed
 *
 *	Called just before performing EH actions to clear related bits
 *	in @link->eh_info such that eh actions are not unnecessarily
 *	repeated.
 *
 *	LOCKING:
 *	None.
 */
void ata_eh_about_to_do(struct ata_link *link, struct ata_device *dev,
			unsigned int action)
{
	struct ata_port *ap = link->ap;
	struct ata_eh_info *ehi = &link->eh_info;
	struct ata_eh_context *ehc = &link->eh_context;
	unsigned long flags;

	spin_lock_irqsave(ap->lock, flags);

	/* Reset is represented by combination of actions and EHI
	 * flags.  Suck in all related bits before clearing eh_info to
	 * avoid losing requested action.
	 */
	if (action & ATA_EH_RESET_MASK) {
		ehc->i.action |= ehi->action & ATA_EH_RESET_MASK;
		ehc->i.flags |= ehi->flags & ATA_EHI_RESET_MODIFIER_MASK;

		/* make sure all reset actions are cleared & clear EHI flags */
		action |= ATA_EH_RESET_MASK;
		ehi->flags &= ~ATA_EHI_RESET_MODIFIER_MASK;
	}

	ata_eh_clear_action(link, dev, ehi, action);

	if (!(ehc->i.flags & ATA_EHI_QUIET))
		ap->pflags |= ATA_PFLAG_RECOVERED;

	spin_unlock_irqrestore(ap->lock, flags);
}

/**
 *	ata_eh_done - EH action complete
*	@ap: target ATA port
 *	@dev: target ATA dev for per-dev action (can be NULL)
 *	@action: action just completed
 *
 *	Called right after performing EH actions to clear related bits
 *	in @link->eh_context.
 *
 *	LOCKING:
 *	None.
 */
void ata_eh_done(struct ata_link *link, struct ata_device *dev,
		 unsigned int action)
{
	struct ata_eh_context *ehc = &link->eh_context;

	/* if reset is complete, clear all reset actions & reset modifier */
	if (action & ATA_EH_RESET_MASK) {
		action |= ATA_EH_RESET_MASK;
		ehc->i.flags &= ~ATA_EHI_RESET_MODIFIER_MASK;
	}

	ata_eh_clear_action(link, dev, &ehc->i, action);
}

/**
 *	ata_err_string - convert err_mask to descriptive string
 *	@err_mask: error mask to convert to string
 *
 *	Convert @err_mask to descriptive string.  Errors are
 *	prioritized according to severity and only the most severe
 *	error is reported.
 *
 *	LOCKING:
 *	None.
 *
 *	RETURNS:
 *	Descriptive string for @err_mask
 */
static const char * ata_err_string(unsigned int err_mask)
{
	if (err_mask & AC_ERR_HOST_BUS)
		return "host bus error";
	if (err_mask & AC_ERR_ATA_BUS)
		return "ATA bus error";
	if (err_mask & AC_ERR_TIMEOUT)
		return "timeout";
	if (err_mask & AC_ERR_HSM)
		return "HSM violation";
	if (err_mask & AC_ERR_SYSTEM)
		return "internal error";
	if (err_mask & AC_ERR_MEDIA)
		return "media error";
	if (err_mask & AC_ERR_INVALID)
		return "invalid argument";
	if (err_mask & AC_ERR_DEV)
		return "device error";
	return "unknown error";
}

/**
 *	ata_read_log_page - read a specific log page
 *	@dev: target device
 *	@page: page to read
 *	@buf: buffer to store read page
 *	@sectors: number of sectors to read
 *
 *	Read log page using READ_LOG_EXT command.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 *
 *	RETURNS:
 *	0 on success, AC_ERR_* mask otherwise.
 */
static unsigned int ata_read_log_page(struct ata_device *dev,
				      u8 page, void *buf, unsigned int sectors)
{
	struct ata_taskfile tf;
	unsigned int err_mask;

	DPRINTK("read log page - page %d\n", page);

	ata_tf_init(dev, &tf);
	tf.command = ATA_CMD_READ_LOG_EXT;
	tf.lbal = page;
	tf.nsect = sectors;
	tf.hob_nsect = sectors >> 8;
	tf.flags |= ATA_TFLAG_ISADDR | ATA_TFLAG_LBA48 | ATA_TFLAG_DEVICE;
	tf.protocol = ATA_PROT_PIO;

	err_mask = ata_exec_internal(dev, &tf, NULL, DMA_FROM_DEVICE,
				     buf, sectors * ATA_SECT_SIZE);

	DPRINTK("EXIT, err_mask=%x\n", err_mask);
	return err_mask;
}

/**
 *	ata_eh_read_log_10h - Read log page 10h for NCQ error details
 *	@dev: Device to read log page 10h from
 *	@tag: Resulting tag of the failed command
 *	@tf: Resulting taskfile registers of the failed command
 *
 *	Read log page 10h to obtain NCQ error details and clear error
 *	condition.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 *
 *	RETURNS:
 *	0 on success, -errno otherwise.
 */
static int ata_eh_read_log_10h(struct ata_device *dev,
			       int *tag, struct ata_taskfile *tf)
{
	u8 *buf = dev->link->ap->sector_buf;
	unsigned int err_mask;
	u8 csum;
	int i;

	err_mask = ata_read_log_page(dev, ATA_LOG_SATA_NCQ, buf, 1);
	if (err_mask)
		return -EIO;

	csum = 0;
	for (i = 0; i < ATA_SECT_SIZE; i++)
		csum += buf[i];
	if (csum)
		ata_dev_printk(dev, KERN_WARNING,
			       "invalid checksum 0x%x on log page 10h\n", csum);

	if (buf[0] & 0x80)
		return -ENOENT;

	*tag = buf[0] & 0x1f;

	tf->command = buf[2];
	tf->feature = buf[3];
	tf->lbal = buf[4];
	tf->lbam = buf[5];
	tf->lbah = buf[6];
	tf->device = buf[7];
	tf->hob_lbal = buf[8];
	tf->hob_lbam = buf[9];
	tf->hob_lbah = buf[10];
	tf->nsect = buf[12];
	tf->hob_nsect = buf[13];

	return 0;
}

/**
 *	atapi_eh_request_sense - perform ATAPI REQUEST_SENSE
 *	@dev: device to perform REQUEST_SENSE to
 *	@sense_buf: result sense data buffer (SCSI_SENSE_BUFFERSIZE bytes long)
 *
 *	Perform ATAPI REQUEST_SENSE after the device reported CHECK
 *	SENSE.  This function is EH helper.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 *
 *	RETURNS:
 *	0 on success, AC_ERR_* mask on failure
 */
static unsigned int atapi_eh_request_sense(struct ata_device *dev,
					   unsigned char *sense_buf)
{
	struct ata_port *ap = dev->link->ap;
	struct ata_taskfile tf;
	u8 cdb[ATAPI_CDB_LEN];

	DPRINTK("ATAPI request sense\n");

	ata_tf_init(dev, &tf);

	/* FIXME: is this needed? */
	memset(sense_buf, 0, SCSI_SENSE_BUFFERSIZE);

	/* XXX: why tf_read here? */
	ap->ops->tf_read(ap, &tf);

	/* fill these in, for the case where they are -not- overwritten */
	sense_buf[0] = 0x70;
	sense_buf[2] = tf.feature >> 4;

	memset(cdb, 0, ATAPI_CDB_LEN);
	cdb[0] = REQUEST_SENSE;
	cdb[4] = SCSI_SENSE_BUFFERSIZE;

	tf.flags |= ATA_TFLAG_ISADDR | ATA_TFLAG_DEVICE;
	tf.command = ATA_CMD_PACKET;

#if !defined (CONFIG_MIPS_BCM_NDVD)
	/* is it pointless to prefer PIO for "safety reasons"? */
	if (ap->flags & ATA_FLAG_PIO_DMA) {
		tf.protocol = ATA_PROT_ATAPI_DMA;
		tf.feature |= ATAPI_PKT_DMA;
	}
	else
#endif
	{
		tf.protocol = ATA_PROT_ATAPI;
		tf.lbam = (8 * 1024) & 0xff;
		tf.lbah = (8 * 1024) >> 8;
	}

	return ata_exec_internal(dev, &tf, cdb, DMA_FROM_DEVICE,
				 sense_buf, SCSI_SENSE_BUFFERSIZE);
}

/**
 *	ata_eh_analyze_serror - analyze SError for a failed port
 *	@link: ATA link to analyze SError for
 *
 *	Analyze SError if available and further determine cause of
 *	failure.
 *
 *	LOCKING:
 *	None.
 */
static void ata_eh_analyze_serror(struct ata_link *link)
{
	struct ata_eh_context *ehc = &link->eh_context;
	u32 serror = ehc->i.serror;
	unsigned int err_mask = 0, action = 0;
	u32 hotplug_mask;

	if (serror & SERR_PERSISTENT) {
		err_mask |= AC_ERR_ATA_BUS;
		action |= ATA_EH_HARDRESET;
	}
	if (serror &
	    (SERR_DATA_RECOVERED | SERR_COMM_RECOVERED | SERR_DATA)) {
		err_mask |= AC_ERR_ATA_BUS;
		action |= ATA_EH_SOFTRESET;
	}
	if (serror & SERR_PROTOCOL) {
		err_mask |= AC_ERR_HSM;
		action |= ATA_EH_SOFTRESET;
	}
	if (serror & SERR_INTERNAL) {
		err_mask |= AC_ERR_SYSTEM;
		action |= ATA_EH_SOFTRESET;
	}

	/* Determine whether a hotplug event has occurred.  Both
	 * SError.N/X are considered hotplug events for enabled links.
	 * For disabled host links, hp-poll is responsible for hotplug
	 * event detection and thus both N/X bits are ignored.  For
	 * disabled PMP links, only N bit is considered as X bit is
	 * left at 1 for link plugging.
	 */
	hotplug_mask = 0;

	if (!(link->flags & ATA_LFLAG_DISABLED))
		hotplug_mask = SERR_PHYRDY_CHG | SERR_DEV_XCHG;
	else if (!ata_is_host_link(link))
		hotplug_mask = SERR_PHYRDY_CHG;

	if (serror & hotplug_mask)
		ata_ehi_hotplugged(&ehc->i);

	ehc->i.err_mask |= err_mask;
	ehc->i.action |= action;
}

/**
 *	ata_eh_analyze_ncq_error - analyze NCQ error
 *	@link: ATA link to analyze NCQ error for
 *
 *	Read log page 10h, determine the offending qc and acquire
 *	error status TF.  For NCQ device errors, all LLDDs have to do
 *	is setting AC_ERR_DEV in ehi->err_mask.  This function takes
 *	care of the rest.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 */
static void ata_eh_analyze_ncq_error(struct ata_link *link)
{
	struct ata_port *ap = link->ap;
	struct ata_eh_context *ehc = &link->eh_context;
	struct ata_device *dev = link->device;
	struct ata_queued_cmd *qc;
	struct ata_taskfile tf;
	int tag, rc;

	/* if frozen, we can't do much */
	if (ap->pflags & ATA_PFLAG_FROZEN)
		return;

	/* is it NCQ device error? */
	if (!link->sactive || !(ehc->i.err_mask & AC_ERR_DEV))
		return;

	/* has LLDD analyzed already? */
	for (tag = 0; tag < ATA_MAX_QUEUE; tag++) {
		qc = __ata_qc_from_tag(ap, tag);

		if (!(qc->flags & ATA_QCFLAG_FAILED))
			continue;

		if (qc->err_mask)
			return;
	}

	/* okay, this error is ours */
	rc = ata_eh_read_log_10h(dev, &tag, &tf);
	if (rc) {
		ata_link_printk(link, KERN_ERR, "failed to read log page 10h "
				"(errno=%d)\n", rc);
		return;
	}

	if (!(link->sactive & (1 << tag))) {
		ata_link_printk(link, KERN_ERR, "log page 10h reported "
				"inactive tag %d\n", tag);
		return;
	}

	/* we've got the perpetrator, condemn it */
	qc = __ata_qc_from_tag(ap, tag);
	memcpy(&qc->result_tf, &tf, sizeof(tf));
	qc->err_mask |= AC_ERR_DEV;
	ehc->i.err_mask &= ~AC_ERR_DEV;
}

/**
 *	ata_eh_analyze_tf - analyze taskfile of a failed qc
 *	@qc: qc to analyze
 *	@tf: Taskfile registers to analyze
 *
 *	Analyze taskfile of @qc and further determine cause of
 *	failure.  This function also requests ATAPI sense data if
 *	avaliable.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 *
 *	RETURNS:
 *	Determined recovery action
 */
static unsigned int ata_eh_analyze_tf(struct ata_queued_cmd *qc,
				      const struct ata_taskfile *tf)
{
	unsigned int tmp, action = 0;
	u8 stat = tf->command, err = tf->feature;

	if ((stat & (ATA_BUSY | ATA_DRQ | ATA_DRDY)) != ATA_DRDY) {
		qc->err_mask |= AC_ERR_HSM;
		return ATA_EH_SOFTRESET;
	}

	if (!(qc->err_mask & AC_ERR_DEV))
		return 0;

	switch (qc->dev->class) {
	case ATA_DEV_ATA:
		if (err & ATA_ICRC)
			qc->err_mask |= AC_ERR_ATA_BUS;
		if (err & ATA_UNC)
			qc->err_mask |= AC_ERR_MEDIA;
		if (err & ATA_IDNF)
			qc->err_mask |= AC_ERR_INVALID;
		break;

	case ATA_DEV_ATAPI:
		tmp = atapi_eh_request_sense(qc->dev,
					     qc->scsicmd->sense_buffer);
		if (!tmp) {
			/* ATA_QCFLAG_SENSE_VALID is used to tell
			 * atapi_qc_complete() that sense data is
			 * already valid.
			 *
			 * TODO: interpret sense data and set
			 * appropriate err_mask.
			 */
			qc->flags |= ATA_QCFLAG_SENSE_VALID;
		} else
			qc->err_mask |= tmp;
	}

	if (qc->err_mask & (AC_ERR_HSM | AC_ERR_TIMEOUT | AC_ERR_ATA_BUS))
		action |= ATA_EH_SOFTRESET;

	return action;
}

static int ata_eh_categorize_ering_entry(struct ata_ering_entry *ent)
{
	if (ent->err_mask & (AC_ERR_ATA_BUS | AC_ERR_TIMEOUT))
		return 1;

	if (ent->is_io) {
		if (ent->err_mask & AC_ERR_HSM)
			return 1;
		if ((ent->err_mask &
		     (AC_ERR_DEV|AC_ERR_MEDIA|AC_ERR_INVALID)) == AC_ERR_DEV)
			return 2;
	}

	return 0;
}

struct speed_down_needed_arg {
	u64 since;
	int nr_errors[3];
};

static int speed_down_needed_cb(struct ata_ering_entry *ent, void *void_arg)
{
	struct speed_down_needed_arg *arg = void_arg;

	if (ent->timestamp < arg->since)
		return -1;

	arg->nr_errors[ata_eh_categorize_ering_entry(ent)]++;
	return 0;
}

/**
 *	ata_eh_speed_down_needed - Determine wheter speed down is necessary
 *	@dev: Device of interest
 *
 *	This function examines error ring of @dev and determines
 *	whether speed down is necessary.  Speed down is necessary if
 *	there have been more than 3 of Cat-1 errors or 10 of Cat-2
 *	errors during last 15 minutes.
 *
 *	Cat-1 errors are ATA_BUS, TIMEOUT for any command and HSM
 *	violation for known supported commands.
 *
 *	Cat-2 errors are unclassified DEV error for known supported
 *	command.
 *
 *	LOCKING:
 *	Inherited from caller.
 *
 *	RETURNS:
 *	1 if speed down is necessary, 0 otherwise
 */
static int ata_eh_speed_down_needed(struct ata_device *dev)
{
	const u64 interval = 15LLU * 60 * HZ;
	static const int err_limits[3] = { -1, 3, 10 };
	struct speed_down_needed_arg arg;
	struct ata_ering_entry *ent;
	int err_cat;
	u64 j64;

	ent = ata_ering_top(&dev->ering);
	if (!ent)
		return 0;

	err_cat = ata_eh_categorize_ering_entry(ent);
	if (err_cat == 0)
		return 0;

	memset(&arg, 0, sizeof(arg));

	j64 = get_jiffies_64();
	if (j64 >= interval)
		arg.since = j64 - interval;
	else
		arg.since = 0;

	ata_ering_map(&dev->ering, speed_down_needed_cb, &arg);

	return arg.nr_errors[err_cat] > err_limits[err_cat];
}

/**
 *	ata_eh_speed_down - record error and speed down if necessary
 *	@dev: Failed device
 *	@is_io: Did the device fail during normal IO?
 *	@err_mask: err_mask of the error
 *
 *	Record error and examine error history to determine whether
 *	adjusting transmission speed is necessary.  It also sets
 *	transmission limits appropriately if such adjustment is
 *	necessary.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 *
 *	RETURNS:
 *	0 on success, -errno otherwise
 */
static int ata_eh_speed_down(struct ata_device *dev, int is_io,
			     unsigned int err_mask)
{
	if (!err_mask)
		return 0;

	/* record error and determine whether speed down is necessary */
	ata_ering_record(&dev->ering, is_io, err_mask);

	if (!ata_eh_speed_down_needed(dev))
		return 0;

	/* speed down SATA link speed if possible */
	if (sata_down_spd_limit(dev->link) == 0)
		return ATA_EH_HARDRESET;

	/* lower transfer mode */
	if (ata_down_xfermask_limit(dev, 0) == 0)
		return ATA_EH_SOFTRESET;

	ata_dev_printk(dev, KERN_ERR,
		       "speed down requested but no transfer mode left\n");
	return 0;
}

/**
 *	ata_eh_link_autopsy - analyze error and determine recovery action
 *	@link: host link to perform autopsy on
 *
 *	Analyze why @link failed and determine which recovery actions
 *	are needed.  This function also sets more detailed AC_ERR_*
 *	values and fills sense data for ATAPI CHECK SENSE.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 */
static void ata_eh_link_autopsy(struct ata_link *link)
{
	struct ata_port *ap = link->ap;
	struct ata_eh_context *ehc = &link->eh_context;
	unsigned int all_err_mask = 0;
	int tag, is_io = 0;
	u32 serror;
	int rc;

	DPRINTK("ENTER\n");

	if (ehc->i.flags & ATA_EHI_NO_AUTOPSY)
		return;

	/* obtain and analyze SError */
	rc = sata_scr_read(link, SCR_ERROR, &serror);
	if (rc == 0) {
		ehc->i.serror |= serror;
		ata_eh_analyze_serror(link);
	} else if (rc != -EOPNOTSUPP) {
		/* hmmm, assume the worst and generate hotplug event */
		__ata_ehi_hotplugged(&ehc->i);
	}

	/* analyze NCQ failure */
	ata_eh_analyze_ncq_error(link);

	/* any real error trumps AC_ERR_OTHER */
	if (ehc->i.err_mask & ~AC_ERR_OTHER)
		ehc->i.err_mask &= ~AC_ERR_OTHER;

	all_err_mask |= ehc->i.err_mask;

	for (tag = 0; tag < ATA_MAX_QUEUE; tag++) {
		struct ata_queued_cmd *qc = __ata_qc_from_tag(ap, tag);

		if (!(qc->flags & ATA_QCFLAG_FAILED) || qc->dev->link != link)
			continue;

		/* inherit upper level err_mask */
		qc->err_mask |= ehc->i.err_mask;

		/* analyze TF */
		ehc->i.action |= ata_eh_analyze_tf(qc, &qc->result_tf);

		/* DEV errors are probably spurious in case of ATA_BUS error */
		if (qc->err_mask & AC_ERR_ATA_BUS)
			qc->err_mask &= ~(AC_ERR_DEV | AC_ERR_MEDIA |
					  AC_ERR_INVALID);

		/* any real error trumps unknown error */
		if (qc->err_mask & ~AC_ERR_OTHER)
			qc->err_mask &= ~AC_ERR_OTHER;

		/* SENSE_VALID trumps dev/unknown error and revalidation */
		if (qc->flags & ATA_QCFLAG_SENSE_VALID) {
			qc->err_mask &= ~(AC_ERR_DEV | AC_ERR_OTHER);
			ehc->i.action &= ~ATA_EH_REVALIDATE;
		}

		/* accumulate error info */
		ehc->i.dev = qc->dev;
		all_err_mask |= qc->err_mask;
		if (qc->flags & ATA_QCFLAG_IO)
			is_io = 1;
	}

	/* enforce default EH actions */
	if (ap->pflags & ATA_PFLAG_FROZEN ||
	    all_err_mask & (AC_ERR_HSM | AC_ERR_TIMEOUT))
		ehc->i.action |= ATA_EH_SOFTRESET;
	else if (all_err_mask)
		ehc->i.action |= ATA_EH_REVALIDATE;

	/* if we have offending qcs and the associated failed device */
	if (ehc->i.dev) {
		/* speed down */
		ehc->i.action |= ata_eh_speed_down(ehc->i.dev, is_io,
						   all_err_mask);

		/* perform per-dev EH action only on the offending device */
		ehc->i.dev_action[ehc->i.dev->devno] |=
			ehc->i.action & ATA_EH_PERDEV_MASK;
		ehc->i.action &= ~ATA_EH_PERDEV_MASK;
	}

	DPRINTK("EXIT\n");
}

/**
 *	ata_eh_autopsy - analyze error and determine recovery action
 *	@ap: host port to perform autopsy on
 *
 *	Analyze all links of @ap and determine why they failed and
 *	which recovery actions are needed.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 */
void ata_eh_autopsy(struct ata_port *ap)
{
	struct ata_link *link;

	__ata_port_for_each_link(link, ap)
		ata_eh_link_autopsy(link);
}

/**
 *	ata_eh_link_report - report error handling to user
 *	@link: ATA link EH is going on
 *
 *	Report EH to user.
 *
 *	LOCKING:
 *	None.
 */
static void ata_eh_link_report(struct ata_link *link)
{
	struct ata_port *ap = link->ap;
	struct ata_eh_context *ehc = &link->eh_context;
	const char *frozen, *desc;
	int tag, nr_failed = 0;

	desc = NULL;
	if (ehc->i.desc[0] != '\0')
		desc = ehc->i.desc;

	for (tag = 0; tag < ATA_MAX_QUEUE; tag++) {
		struct ata_queued_cmd *qc = __ata_qc_from_tag(ap, tag);

		if (!(qc->flags & ATA_QCFLAG_FAILED) || qc->dev->link != link)
			continue;
		if (qc->flags & ATA_QCFLAG_SENSE_VALID && !qc->err_mask)
			continue;

		nr_failed++;
	}

	if (!nr_failed && !ehc->i.err_mask)
		return;

	frozen = "";
	if (ap->pflags & ATA_PFLAG_FROZEN)
		frozen = " frozen";

	if (ehc->i.dev) {
		ata_dev_printk(ehc->i.dev, KERN_ERR, "exception Emask 0x%x "
			       "SAct 0x%x SErr 0x%x action 0x%x%s\n",
			       ehc->i.err_mask, link->sactive,
			       ehc->i.serror, ehc->i.action, frozen);
		if (desc)
			ata_dev_printk(ehc->i.dev, KERN_ERR, "(%s)\n", desc);
	} else {
		ata_link_printk(link, KERN_ERR, "exception Emask 0x%x "
				"SAct 0x%x SErr 0x%x action 0x%x%s\n",
				ehc->i.err_mask, link->sactive,
				ehc->i.serror, ehc->i.action, frozen);
		if (desc)
			ata_link_printk(link, KERN_ERR, "(%s)\n", desc);
	}

	for (tag = 0; tag < ATA_MAX_QUEUE; tag++) {
		struct ata_queued_cmd *qc = __ata_qc_from_tag(ap, tag);

		if (!(qc->flags & ATA_QCFLAG_FAILED) ||
		    qc->dev->link != link || !qc->err_mask)
			continue;

		ata_dev_printk(qc->dev, KERN_ERR, "tag %d cmd 0x%x "
			       "Emask 0x%x stat 0x%x err 0x%x (%s)\n",
			       qc->tag, qc->tf.command, qc->err_mask,
			       qc->result_tf.command, qc->result_tf.feature,
			       ata_err_string(qc->err_mask));
	}
}

/**
 *	ata_eh_report - report error handling to user
 *	@ap: ATA port to report EH about
 *
 *	Report EH to user.
 *
 *	LOCKING:
 *	None.
 */
void ata_eh_report(struct ata_port *ap)
{
	struct ata_link *link;

	__ata_port_for_each_link(link, ap)
		ata_eh_link_report(link);
}

static int ata_do_reset(struct ata_link *link, ata_reset_fn_t reset,
			unsigned int *classes)
{
	struct ata_device *dev;
	int rc;

	ata_link_for_each_dev(dev, link)
		classes[dev->devno] = ATA_DEV_UNKNOWN;

	rc = reset(link, classes);
	if (rc)
		return rc;

	/* If any class isn't ATA_DEV_UNKNOWN, consider classification
	 * is complete and convert all ATA_DEV_UNKNOWN to
	 * ATA_DEV_NONE.
	 */
	ata_link_for_each_dev(dev, link)
		if (classes[dev->devno] != ATA_DEV_UNKNOWN)
			break;

	if (dev) {
		ata_link_for_each_dev(dev, link) {
			if (classes[dev->devno] == ATA_DEV_UNKNOWN)
				classes[dev->devno] = ATA_DEV_NONE;
		}
	}

	return 0;
}

static int ata_eh_followup_srst_needed(struct ata_port *ap,
				       int rc, int classify,
				       const unsigned int *classes)
{
	if (rc == -EAGAIN)
		return 1;
	if (rc != 0)
		return 0;
	if (ap->flags & ATA_FLAG_PMP)
		return 1;
	if (classify && classes[0] == ATA_DEV_UNKNOWN)
		return 1;
	return 0;
}

int ata_eh_reset(struct ata_link *link, int classify,
		 ata_prereset_fn_t prereset, ata_reset_fn_t softreset,
		 ata_reset_fn_t hardreset, ata_postreset_fn_t postreset)
{
	struct ata_eh_context *ehc = &link->eh_context;
	unsigned int *classes = ehc->classes;
	int tries = link->reset_tries;
	int verbose = !(ehc->i.flags & ATA_EHI_QUIET);
	struct ata_device *dev;
	unsigned int action;
	ata_reset_fn_t reset;
	int did_followup_srst, rc;

	/* about to reset */
	ata_eh_about_to_do(link, NULL, ehc->i.action & ATA_EH_RESET_MASK);

	/* Determine which reset to use and record in ehc->i.action.
	 * prereset() may examine and modify it.
	 */
	action = ehc->i.action;
	ehc->i.action &= ~ATA_EH_RESET_MASK;
	if (softreset && (!hardreset || (!sata_set_spd_needed(link) &&
					 !(action & ATA_EH_HARDRESET))))
		ehc->i.action |= ATA_EH_SOFTRESET;
	else
		ehc->i.action |= ATA_EH_HARDRESET;

	if (prereset) {
		rc = prereset(link);
		if (rc) {
			if (rc == -ENOENT) {
				ata_link_printk(link, KERN_DEBUG,
						"link disabled. ignoring.\n");
				ehc->i.action &= ~ATA_EH_RESET_MASK;
			} else
				ata_link_printk(link, KERN_ERR,
					"prereset failed (errno=%d)\n", rc);
			return rc;
		}
	}

	/* prereset() might have modified ehc->i.action */
	if (ehc->i.action & ATA_EH_HARDRESET)
		reset = hardreset;
	else if (ehc->i.action & ATA_EH_SOFTRESET)
		reset = softreset;
	else {
		/* prereset told us not to reset, bang classes and return */
		ata_link_for_each_dev(dev, link)
			classes[dev->devno] = ATA_DEV_NONE;
		return 0;
	}

	/* did prereset() screw up?  if so, fix up to avoid oopsing */
	if (!reset) {
		ata_link_printk(link, KERN_ERR, "BUG: prereset() requested "
				"invalid reset type\n");
		if (softreset)
			reset = softreset;
		else
			reset = hardreset;
	}

 retry:
	/* shut up during boot probing */
	if (verbose)
		ata_link_printk(link, KERN_INFO, "%s resetting port\n",
				reset == softreset ? "soft" : "hard");

	/* mark that this EH session started with reset */
	ehc->i.flags |= ATA_EHI_DID_RESET;

	rc = ata_do_reset(link, reset, classes);

	did_followup_srst = 0;
	if (reset == hardreset &&
	    ata_eh_followup_srst_needed(link->ap, rc, classify, classes)) {
		/* okay, let's do follow-up softreset */
		did_followup_srst = 1;
		reset = softreset;

		if (!reset) {
			ata_link_printk(link, KERN_ERR,
					"follow-up softreset required "
					"but no softreset avaliable\n");
			return -EINVAL;
		}

		ata_eh_about_to_do(link, NULL, ATA_EH_RESET_MASK);
		rc = ata_do_reset(link, reset, classes);

		if (rc == 0 && classify &&
		    classes[0] == ATA_DEV_UNKNOWN) {
			ata_link_printk(link, KERN_ERR,
					"classification failed\n");
			return -EINVAL;
		}
	}

	if (rc && --tries) {
		const char *type;

		if (reset == softreset) {
			if (did_followup_srst)
				type = "follow-up soft";
			else
				type = "soft";
		} else
			type = "hard";

		ata_link_printk(link, KERN_WARNING,
				"%sreset failed, retrying in 5 secs\n", type);
		ssleep(5);

		if (reset == hardreset)
			sata_down_spd_limit(link);
		if (hardreset)
			reset = hardreset;
		goto retry;
	}

	if (rc == 0) {
		/* After the reset, the device state is PIO 0 and the
		 * controller state is undefined.  Record the mode.
		 */
		ata_link_for_each_dev(dev, link)
			dev->pio_mode = XFER_PIO_0;

		if (postreset)
			postreset(link, classes);

		/* reset successful, schedule revalidation */
		ata_eh_done(link, NULL, ehc->i.action & ATA_EH_RESET_MASK);
		ehc->i.action |= ATA_EH_REVALIDATE;
	}

	return rc;
}

static int ata_eh_revalidate_and_attach(struct ata_link *link,
					struct ata_device **r_failed_dev)
{
	struct ata_port *ap = link->ap;
	struct ata_eh_context *ehc = &link->eh_context;
	struct ata_device *dev;
	unsigned long flags;
	int rc = 0;

	DPRINTK("ENTER\n");

	ata_link_for_each_dev(dev, link) {
		unsigned int action = ata_eh_dev_action(dev);

		if (action & ATA_EH_REVALIDATE && ata_dev_ready(dev)) {
			WARN_ON(dev->class == ATA_DEV_PMP);

			if (ata_link_offline(dev->link)) {
				rc = -EIO;
				break;
			}

			ata_eh_about_to_do(link, dev, ATA_EH_REVALIDATE);
			rc = ata_dev_revalidate(dev, ehc->classes[dev->devno],
					ehc->i.flags & ATA_EHI_DID_RESET);
			if (rc)
				break;

			ata_eh_done(link, dev, ATA_EH_REVALIDATE);

			/* schedule the scsi_rescan_device() here */
			queue_work(ata_aux_wq, &(ap->scsi_rescan_task));
		} else if (dev->class == ATA_DEV_UNKNOWN &&
			   ehc->tries[dev->devno] &&
			   ata_class_enabled(ehc->classes[dev->devno])) {
			dev->class = ehc->classes[dev->devno];

			if (dev->class == ATA_DEV_PMP) {
				rc = sata_pmp_attach(dev);
				if (rc)
					dev->class = ATA_DEV_UNKNOWN;
				break;
			}

			rc = ata_dev_read_id(dev, &dev->class, 1, dev->id);
			if (rc == 0)
				rc = ata_dev_configure(dev, 1);

			if (rc) {
				dev->class = ATA_DEV_UNKNOWN;
				break;
			}

			spin_lock_irqsave(ap->lock, flags);
			ap->pflags |= ATA_PFLAG_SCSI_HOTPLUG;
			spin_unlock_irqrestore(ap->lock, flags);
		}
	}

	if (rc)
		*r_failed_dev = dev;

	DPRINTK("EXIT\n");
	return rc;
}

/**
 *	ata_eh_suspend - handle suspend EH action
 *	@link: target link
 *	@r_failed_dev: result parameter to indicate failing device
 *
 *	Handle suspend EH action.  Disk devices are spinned down and
 *	other types of devices are just marked suspended.  Once
 *	suspended, no EH action to the device is allowed until it is
 *	resumed.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 *
 *	RETURNS:
 *	0 on success, -errno otherwise
 */
static int ata_eh_suspend(struct ata_link *link,
			  struct ata_device **r_failed_dev)
{
	struct ata_port *ap = link->ap;
	struct ata_device *dev;
	int rc = 0;

	DPRINTK("ENTER\n");

	ata_link_for_each_dev(dev, link) {
		unsigned int action = ata_eh_dev_action(dev);
		unsigned int err_mask;
		unsigned long flags;

		if (!ata_dev_enabled(dev) || !(action & ATA_EH_SUSPEND))
			continue;

		WARN_ON(dev->flags & ATA_DFLAG_SUSPENDED);

		ata_eh_about_to_do(link, dev, ATA_EH_SUSPEND);

		if (dev->class == ATA_DEV_ATA && !(action & ATA_EH_PM_FREEZE)) {
			/* flush cache */
			rc = ata_flush_cache(dev);
			if (rc)
				break;

			/* spin down */
			err_mask = ata_do_simple_cmd(dev, ATA_CMD_STANDBYNOW1);
			if (err_mask) {
				ata_dev_printk(dev, KERN_ERR, "failed to "
					       "spin down (err_mask=0x%x)\n",
					       err_mask);
				rc = -EIO;
				break;
			}
		}

		spin_lock_irqsave(ap->lock, flags);
		dev->flags |= ATA_DFLAG_SUSPENDED;
		spin_unlock_irqrestore(ap->lock, flags);

		ata_eh_done(link, dev, ATA_EH_SUSPEND);
	}

	if (rc)
		*r_failed_dev = dev;

	DPRINTK("EXIT\n");
	return 0;
}

/**
 *	ata_eh_prep_resume - prep for resume EH action
 *	@link: target link
 *
 *	Clear SUSPENDED in preparation for scheduled resume actions.
 *	This allows other parts of EH to access the devices being
 *	resumed.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 */
static void ata_eh_prep_resume(struct ata_link *link)
{
	struct ata_port *ap = link->ap;
	struct ata_device *dev;
	unsigned long flags;

	DPRINTK("ENTER\n");

	ata_link_for_each_dev(dev, link) {
		unsigned int action = ata_eh_dev_action(dev);

		if (!ata_dev_enabled(dev) || !(action & ATA_EH_RESUME))
			continue;

		spin_lock_irqsave(ap->lock, flags);
		dev->flags &= ~ATA_DFLAG_SUSPENDED;
		spin_unlock_irqrestore(ap->lock, flags);
	}

	DPRINTK("EXIT\n");
}

/**
 *	ata_eh_resume - handle resume EH action
 *	@link: target link
 *	@r_failed_dev: result parameter to indicate failing device
 *
 *	Handle resume EH action.  Target devices are already reset and
 *	revalidated.  Spinning up is the only operation left.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 *
 *	RETURNS:
 *	0 on success, -errno otherwise
 */
static int ata_eh_resume(struct ata_link *link,
			 struct ata_device **r_failed_dev)
{
	struct ata_device *dev;
	int rc = 0;

	DPRINTK("ENTER\n");

	ata_link_for_each_dev(dev, link) {
		unsigned int action = ata_eh_dev_action(dev);
		unsigned int err_mask;

		if (!ata_dev_enabled(dev) || !(action & ATA_EH_RESUME))
			continue;

		ata_eh_about_to_do(link, dev, ATA_EH_RESUME);

		if (dev->class == ATA_DEV_ATA && !(action & ATA_EH_PM_FREEZE)) {
			err_mask = ata_do_simple_cmd(dev,
						     ATA_CMD_IDLEIMMEDIATE);
			if (err_mask) {
				ata_dev_printk(dev, KERN_ERR, "failed to "
					       "spin up (err_mask=0x%x)\n",
					       err_mask);
				rc = -EIO;
				break;
			}
		}

		ata_eh_done(link, dev, ATA_EH_RESUME);
	}

	if (rc)
		*r_failed_dev = dev;

	DPRINTK("EXIT\n");
	return 0;
}

static int ata_link_nr_enabled(struct ata_link *link)
{
	struct ata_device *dev;
	int cnt = 0;

	ata_link_for_each_dev(dev, link)
		if (ata_dev_enabled(dev))
			cnt++;
	return cnt;
}

static int ata_port_nr_enabled(struct ata_port *ap)
{
	struct ata_link *link;
	int cnt = 0;

	ata_port_for_each_link(link, ap)
		cnt += ata_link_nr_enabled(link);

	return cnt;
}

static int ata_link_nr_vacant(struct ata_link *link)
{
	struct ata_device *dev;
	int cnt = 0;

	ata_link_for_each_dev(dev, link)
		if (dev->class == ATA_DEV_UNKNOWN)
			cnt++;
	return cnt;
}

static int ata_eh_skip_recovery(struct ata_link *link)
{
	struct ata_eh_context *ehc = &link->eh_context;
	struct ata_device *dev;

	/* skip disabled links */
	if (link->flags & ATA_LFLAG_DISABLED)
		return 1;

	/* skip if all possible devices are suspended */
	ata_link_for_each_dev(dev, link) {
		if (!(dev->flags & ATA_DFLAG_SUSPENDED))
			break;
	}

	if (dev == NULL)
		return 1;

	/* thaw frozen port, resume link and recover failed devices */
	if ((link->ap->pflags & ATA_PFLAG_FROZEN) ||
	    (ehc->i.flags & ATA_EHI_RESUME_LINK) || ata_link_nr_enabled(link))
		return 0;

	/* skip if class codes for all vacant slots are ATA_DEV_NONE */
	ata_link_for_each_dev(dev, link) {
		if (dev->class == ATA_DEV_UNKNOWN &&
		    ehc->classes[dev->devno] != ATA_DEV_NONE)
			return 0;
	}

	return 1;
}

static void ata_eh_handle_dev_fail(struct ata_device *dev, int err,
				   int down_xfermask)
{
	struct ata_link *link = dev->link;
	struct ata_eh_context *ehc = &link->eh_context;

	switch (err) {
	case -ENODEV:
		/* device missing, schedule probing */
		ehc->i.probe_mask |= (1 << dev->devno);
	case -EINVAL:
		ehc->tries[dev->devno] = 0;
		break;
	case -EIO:
		sata_down_spd_limit(link);
	default:
		ehc->tries[dev->devno]--;
		if (down_xfermask &&
		    ata_down_xfermask_limit(dev, ehc->tries[dev->devno] == 1))
			ehc->tries[dev->devno] = 0;
	}

	if (ata_dev_enabled(dev) && !ehc->tries[dev->devno]) {
		/* disable device if it has used up all its chances */
		ata_dev_disable(dev);

		/* detach if offline */
		if (ata_link_offline(link))
			ata_eh_detach_dev(dev);

		/* probe if requested */
		if ((ehc->i.probe_mask & (1 << dev->devno)) &&
		    !(ehc->did_probe_mask & (1 << dev->devno))) {
			ata_eh_detach_dev(dev);
			ata_dev_init(dev);

			ehc->tries[dev->devno] = ATA_EH_DEV_TRIES;
			ehc->did_probe_mask |= (1 << dev->devno);
			ehc->i.action |= ATA_EH_SOFTRESET;
		}
	} else {
		/* soft didn't work?  be haaaaard */
		if (ehc->i.flags & ATA_EHI_DID_RESET)
			ehc->i.action |= ATA_EH_HARDRESET;
		else
			ehc->i.action |= ATA_EH_SOFTRESET;
	}
}

/**
 *	ata_eh_recover - recover host port after error
 *	@ap: host port to recover
 *	@prereset: prereset method (can be NULL)
 *	@softreset: softreset method (can be NULL)
 *	@hardreset: hardreset method (can be NULL)
 *	@postreset: postreset method (can be NULL)
 *	@r_failed_link: out parameter for failed link
 *
 *	This is the alpha and omega, eum and yang, heart and soul of
 *	libata exception handling.  On entry, actions required to
 *	recover each link and hotplug requests are recorded in the
 *	link's eh_context.  This function executes all the operations
 *	with appropriate retrials and fallbacks to resurrect failed
 *	devices, detach goners and greet newcomers.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 *
 *	RETURNS:
 *	0 on success, -errno on failure.
 */
int ata_eh_recover(struct ata_port *ap, ata_prereset_fn_t prereset,
		   ata_reset_fn_t softreset, ata_reset_fn_t hardreset,
		   ata_postreset_fn_t postreset,
		   struct ata_link **r_failed_link)
{
	struct ata_link *link;
	struct ata_device *dev;
	int nr_failed_devs;
	int reset, rc;

	DPRINTK("ENTER\n");

	/* prep for recovery */
	ata_port_for_each_link(link, ap) {
		struct ata_eh_context *ehc = &link->eh_context;

		/* re-enable link? */
		if (ehc->i.action & ATA_EH_ENABLE_LINK) {
			ata_eh_about_to_do(link, NULL, ATA_EH_ENABLE_LINK);
			link->flags &= ~ATA_LFLAG_DISABLED;
			ata_eh_done(link, NULL, ATA_EH_ENABLE_LINK);
		}

		ata_link_for_each_dev(dev, link) {
			ehc->tries[dev->devno] = ATA_EH_DEV_TRIES;

			/* process hotplug request */
			if (dev->flags & ATA_DFLAG_DETACH)
				ata_eh_detach_dev(dev);

			if (!ata_dev_enabled(dev) &&
			    (ehc->i.probe_mask & (1 << dev->devno) &&
			     !(ehc->did_probe_mask & (1 << dev->devno)))) {
				ata_eh_detach_dev(dev);
				ata_dev_init(dev);
				ehc->did_probe_mask |= (1 << dev->devno);
				ehc->i.action |= ATA_EH_SOFTRESET;
			}
		}
	}

 retry:
	rc = 0;
	nr_failed_devs = 0;
	reset = 0;

	/* if UNLOADING, finish immediately */
	if (ap->pflags & ATA_PFLAG_UNLOADING)
		goto out;

	/* prep for EH */
	ata_port_for_each_link(link, ap) {
		struct ata_eh_context *ehc = &link->eh_context;

		/* prep for resume */
		ata_eh_prep_resume(link);

		/* skip EH if possible. */
		if (ata_eh_skip_recovery(link))
			ehc->i.action = 0;

		/* do we need to reset? */
		if (ehc->i.action & ATA_EH_RESET_MASK)
			reset = 1;

		ata_link_for_each_dev(dev, link)
			ehc->classes[dev->devno] = ATA_DEV_UNKNOWN;
	}

	/* reset */
	if (reset) {
		ata_eh_freeze_port(ap);

		ata_port_for_each_link(link, ap) {
			struct ata_eh_context *ehc = &link->eh_context;

			if (!(ehc->i.action & ATA_EH_RESET_MASK))
				continue;

			rc = ata_eh_reset(link, ata_link_nr_vacant(link),
					  prereset, softreset, hardreset,
					  postreset);
			if (rc) {
				ata_link_printk(link, KERN_ERR,
						"reset failed, giving up\n");
				goto out;
			}
		}

		ata_eh_thaw_port(ap);
	}

	/* the rest */
	ata_port_for_each_link(link, ap) {
		struct ata_eh_context *ehc = &link->eh_context;
		int down_xfermask = 0;

		/* revalidate existing devices and attach new ones */
		rc = ata_eh_revalidate_and_attach(link, &dev);
		if (rc)
			goto dev_fail;

		/* resume devices */
		rc = ata_eh_resume(link, &dev);
		if (rc)
			goto dev_fail;

		/* if PMP got attached, return, we don't know what to do */
		if (link->device->class == ATA_DEV_PMP) {
			ehc->i.action = 0;
			return 0;
		}

		/* configure transfer mode if the port has been reset */
		if (ehc->i.flags & ATA_EHI_DID_RESET) {
			rc = ata_set_mode(link, &dev);
			if (rc) {
				down_xfermask = 1;
				goto dev_fail;
			}
		}

		/* suspend devices */
		rc = ata_eh_suspend(link, &dev);
		if (rc)
			goto dev_fail;

		/* this link is okay now */
		ehc->i.flags = 0;
		continue;

	dev_fail:
		ata_eh_handle_dev_fail(dev, rc, down_xfermask);
		nr_failed_devs++;

		if (ap->pflags & ATA_PFLAG_FROZEN)
			break;
	}

	if (nr_failed_devs) {
		if (ata_port_nr_enabled(ap)) {
			ata_port_printk(ap, KERN_WARNING, "failed to recover "
					"some devices, retrying in 5 secs\n");
			ssleep(5);
		} else {
			/* no device left, repeat fast */
			msleep(500);
		}

		goto retry;
	}

 out:
	if (rc && r_failed_link)
		*r_failed_link = link;

	DPRINTK("EXIT, rc=%d\n", rc);
	return rc;
}

/**
 *	ata_eh_finish - finish up EH
 *	@ap: host port to finish EH for
 *
 *	Recovery is complete.  Clean up EH states and retry or finish
 *	failed qcs.
 *
 *	LOCKING:
 *	None.
 */
void ata_eh_finish(struct ata_port *ap)
{
	int tag;

	/* retry or finish qcs */
	for (tag = 0; tag < ATA_MAX_QUEUE; tag++) {
		struct ata_queued_cmd *qc = __ata_qc_from_tag(ap, tag);

		if (!(qc->flags & ATA_QCFLAG_FAILED))
			continue;

		if (qc->err_mask) {
			/* FIXME: Once EH migration is complete,
			 * generate sense data in this function,
			 * considering both err_mask and tf.
			 */
			if (qc->err_mask & AC_ERR_INVALID)
				ata_eh_qc_complete(qc);
			else
				ata_eh_qc_retry(qc);
		} else {
			if (qc->flags & ATA_QCFLAG_SENSE_VALID) {
				ata_eh_qc_complete(qc);
			} else {
				/* feed zero TF to sense generation */
				memset(&qc->result_tf, 0, sizeof(qc->result_tf));
				ata_eh_qc_retry(qc);
			}
		}
	}

	/* make sure nr_active_links is zero after EH */
	WARN_ON(ap->nr_active_links);
	ap->nr_active_links = 0;
}

/**
 *	ata_do_eh - do standard error handling
 *	@ap: host port to handle error for
 *	@prereset: prereset method (can be NULL)
 *	@softreset: softreset method (can be NULL)
 *	@hardreset: hardreset method (can be NULL)
 *	@postreset: postreset method (can be NULL)
 *
 *	Perform standard error handling sequence.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 */
void ata_do_eh(struct ata_port *ap, ata_prereset_fn_t prereset,
	       ata_reset_fn_t softreset, ata_reset_fn_t hardreset,
	       ata_postreset_fn_t postreset)
{
	struct ata_device *dev;
	int rc;

	ata_eh_autopsy(ap);
	ata_eh_report(ap);

	rc = ata_eh_recover(ap, prereset, softreset, hardreset, postreset,
			    NULL);
	if (rc) {
		/* recovery failed, activate hp-poll */
		ata_hp_poll_activate(ap);

		ata_link_for_each_dev(dev, &ap->link)
			ata_dev_disable(dev);
	}

	ata_eh_finish(ap);
}

/**
 *	ata_eh_handle_port_suspend - perform port suspend operation
 *	@ap: port to suspend
 *
 *	Suspend @ap.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 */
static void ata_eh_handle_port_suspend(struct ata_port *ap)
{
	unsigned long flags;
	int rc = 0;

	/* are we suspending? */
	spin_lock_irqsave(ap->lock, flags);
	if (!(ap->pflags & ATA_PFLAG_PM_PENDING) ||
	    ap->pm_mesg.event == PM_EVENT_ON) {
		spin_unlock_irqrestore(ap->lock, flags);
		return;
	}
	spin_unlock_irqrestore(ap->lock, flags);

	WARN_ON(ap->pflags & ATA_PFLAG_SUSPENDED);

	/* suspend */
	ata_eh_freeze_port(ap);

	if (ap->ops->port_suspend)
		rc = ap->ops->port_suspend(ap, ap->pm_mesg);

	/* report result */
	spin_lock_irqsave(ap->lock, flags);

	ap->pflags &= ~ATA_PFLAG_PM_PENDING;
	if (rc == 0)
		ap->pflags |= ATA_PFLAG_SUSPENDED;
	else
		ata_port_schedule_eh(ap);

	if (ap->pm_result) {
		*ap->pm_result = rc;
		ap->pm_result = NULL;
	}

	spin_unlock_irqrestore(ap->lock, flags);

	return;
}

/**
 *	ata_eh_handle_port_resume - perform port resume operation
 *	@ap: port to resume
 *
 *	Resume @ap.
 *
 *	This function also waits upto one second until all devices
 *	hanging off this port requests resume EH action.  This is to
 *	prevent invoking EH and thus reset multiple times on resume.
 *
 *	On DPM resume, where some of devices might not be resumed
 *	together, this may delay port resume upto one second, but such
 *	DPM resumes are rare and 1 sec delay isn't too bad.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 */
static void ata_eh_handle_port_resume(struct ata_port *ap)
{
	unsigned long timeout;
	unsigned long flags;
	int rc = 0;

	/* are we resuming? */
	spin_lock_irqsave(ap->lock, flags);
	if (!(ap->pflags & ATA_PFLAG_PM_PENDING) ||
	    ap->pm_mesg.event != PM_EVENT_ON) {
		spin_unlock_irqrestore(ap->lock, flags);
		return;
	}
	spin_unlock_irqrestore(ap->lock, flags);

	/* spurious? */
	if (!(ap->pflags & ATA_PFLAG_SUSPENDED))
		goto done;

	if (ap->ops->port_resume)
		rc = ap->ops->port_resume(ap);

	/* give devices time to request EH */
	timeout = jiffies + HZ; /* 1s max */
	while (1) {
		struct ata_link *link;
		struct ata_device *dev;

		ata_port_for_each_link(link, ap) {
			ata_link_for_each_dev(dev, link) {
				unsigned int action = ata_eh_dev_action(dev);

				if ((dev->flags & ATA_DFLAG_SUSPENDED) &&
				    !(action & ATA_EH_RESUME))
					goto out_of_loop;
			}
		}
	out_of_loop:

		if (link == NULL || time_after(jiffies, timeout))
			break;
		msleep(10);
	}

 done:
	spin_lock_irqsave(ap->lock, flags);
	ap->pflags &= ~(ATA_PFLAG_PM_PENDING | ATA_PFLAG_SUSPENDED);
	if (ap->pm_result) {
		*ap->pm_result = rc;
		ap->pm_result = NULL;
	}
	spin_unlock_irqrestore(ap->lock, flags);
}

static unsigned long ata_hp_poll_delay(void)
{
	unsigned long interval, now;

	/* Group polls to polling interval boundaries to allow cpu to
	 * go idle as much as possible.
	 */
	interval = hotplug_polling_interval * HZ / 1000;
	now = jiffies;
	return roundup(now + 1, interval) - now;
}

/**
 *	ata_hp_poll_activate - activate hotplug polling
 *	@ap: host port to activate hotplug polling for
 *
 *	Activate hotplug probing for @ap.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 */
void ata_hp_poll_activate(struct ata_port *ap)
{
	unsigned long flags;

	if (!ap->ops->hp_poll || (ap->pflags & ATA_PFLAG_HP_POLL))
		return;

	if (ap->ops->hp_poll_activate)
		ap->ops->hp_poll_activate(ap);

	queue_delayed_work(ata_aux_wq, &ap->host->hp_poll_task,
			   ata_hp_poll_delay());
	ap->hp_poll_data = 0;

	spin_lock_irqsave(ap->lock, flags);
	ap->pflags |= ATA_PFLAG_HP_POLL;
	spin_unlock_irqrestore(ap->lock, flags);
}

/**
 *	ata_hp_poll_deactivate - deactivate hotplug polling
 *	@ap: host port to deactivate hotplug polling for
 *
 *	Deactivate hotplug probing for @ap.
 *
 *	LOCKING:
 *	Kernel thread context (may sleep).
 */
void ata_hp_poll_deactivate(struct ata_port *ap)
{
	unsigned long flags;

	if (!(ap->pflags & ATA_PFLAG_HP_POLL))
		return;

	spin_lock_irqsave(ap->lock, flags);
	ap->pflags &= ~ATA_PFLAG_HP_POLL;
	spin_unlock_irqrestore(ap->lock, flags);

	if (ap->ops->hp_poll_deactivate)
		ap->ops->hp_poll_deactivate(ap);
}

void ata_hp_poll_worker(void *data)
{
	struct ata_host *host = data;
	int i, nr_to_poll = 0;
	unsigned long flags;

	spin_lock_irqsave(&host->lock, flags);

	for (i = 0; i < host->n_ports; i++) {
		struct ata_port *ap = host->ports[i];
		int rc;

		if (!(ap->pflags & ATA_PFLAG_HP_POLL))
			continue;

		/* Poll.  Positive return value indicates hotplug
		 * event while negative indicates error condition.
		 */
		rc = ap->ops->hp_poll(ap);
		if (rc) {
			if (rc > 0) {
				ata_ehi_hotplugged(&ap->link.eh_info);
				ata_port_freeze(ap);
			}
			ap->pflags &= ~ATA_PFLAG_HP_POLL;
		} else
			nr_to_poll++;
	}

	spin_unlock_irqrestore(&host->lock, flags);

	if (nr_to_poll)
		queue_delayed_work(ata_aux_wq, &host->hp_poll_task,
				   ata_hp_poll_delay());
}
