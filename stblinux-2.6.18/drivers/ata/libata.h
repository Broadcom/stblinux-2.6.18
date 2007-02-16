/*
 *  libata.h - helper library for ATA
 *
 *  Copyright 2003-2004 Red Hat, Inc.  All rights reserved.
 *  Copyright 2003-2004 Jeff Garzik
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 *  libata documentation is available via 'make {ps|pdf}docs',
 *  as Documentation/DocBook/libata.*
 *
 */

#ifndef __LIBATA_H__
#define __LIBATA_H__

#define DRV_NAME	"libata"
#define DRV_VERSION	"2.00"	/* must be exactly four chars */

struct ata_scsi_args {
	struct ata_device	*dev;
	u16			*id;
	struct scsi_cmnd	*cmd;
	void			(*done)(struct scsi_cmnd *);
};

/* libata-core.c */
extern struct workqueue_struct *ata_aux_wq;
extern int atapi_enabled;
extern int atapi_dmadir;
extern int libata_fua;
extern struct ata_queued_cmd *ata_qc_new_init(struct ata_device *dev);
extern int ata_rwcmd_protocol(struct ata_queued_cmd *qc);
extern void ata_dev_disable(struct ata_device *dev);
extern void ata_port_flush_task(struct ata_port *ap);
extern unsigned ata_exec_internal(struct ata_device *dev,
				  struct ata_taskfile *tf, const u8 *cdb,
				  int dma_dir, void *buf, unsigned int buflen);
extern unsigned int ata_do_simple_cmd(struct ata_device *dev, u8 cmd);
extern int ata_dev_read_id(struct ata_device *dev, unsigned int *p_class,
			   int post_reset, u16 *id);
extern int ata_dev_configure(struct ata_device *dev, int print_info);
extern void sata_print_link_status(struct ata_link *link);
extern int sata_down_spd_limit(struct ata_link *link);
extern int sata_set_spd_needed(struct ata_link *link);
extern int ata_down_xfermask_limit(struct ata_device *dev, int force_pio0);
extern int ata_set_mode(struct ata_link *link,
			struct ata_device **r_failed_dev);
extern void ata_wait_spinup(struct ata_link *link);
extern void ata_qc_free(struct ata_queued_cmd *qc);
extern void ata_qc_issue(struct ata_queued_cmd *qc);
extern void __ata_qc_complete(struct ata_queued_cmd *qc);
extern int ata_check_atapi_dma(struct ata_queued_cmd *qc);
extern void ata_dev_select(struct ata_port *ap, unsigned int device,
                           unsigned int wait, unsigned int can_sleep);
extern void swap_buf_le16(u16 *buf, unsigned int buf_words);
extern int ata_flush_cache(struct ata_device *dev);
extern void ata_dev_init(struct ata_device *dev);
extern void ata_link_init(struct ata_port *ap, struct ata_link *link, int pmp);
extern int sata_link_init_spd_limit(struct ata_link *link);
extern int ata_task_ioctl(struct scsi_device *scsidev, void __user *arg);
extern int ata_cmd_ioctl(struct scsi_device *scsidev, void __user *arg);
extern void ata_port_init(struct ata_port *ap, struct ata_host *host,
			  const struct ata_probe_ent *ent, unsigned int port_no);
extern struct ata_probe_ent *ata_probe_ent_alloc(struct device *dev,
						 const struct ata_port_info *port);


/* libata-scsi.c */
extern struct scsi_transport_template ata_scsi_transport_template;

extern void ata_scsi_scan_host(struct ata_port *ap);
extern int ata_scsi_offline_dev(struct ata_device *dev);
extern void ata_scsi_hotplug(void *data);
extern unsigned int ata_scsiop_inq_std(struct ata_scsi_args *args, u8 *rbuf,
			       unsigned int buflen);

extern unsigned int ata_scsiop_inq_00(struct ata_scsi_args *args, u8 *rbuf,
			      unsigned int buflen);

extern unsigned int ata_scsiop_inq_80(struct ata_scsi_args *args, u8 *rbuf,
			      unsigned int buflen);
extern unsigned int ata_scsiop_inq_83(struct ata_scsi_args *args, u8 *rbuf,
			      unsigned int buflen);
extern unsigned int ata_scsiop_noop(struct ata_scsi_args *args, u8 *rbuf,
			    unsigned int buflen);
extern unsigned int ata_scsiop_sync_cache(struct ata_scsi_args *args, u8 *rbuf,
				  unsigned int buflen);
extern unsigned int ata_scsiop_mode_sense(struct ata_scsi_args *args, u8 *rbuf,
				  unsigned int buflen);
extern unsigned int ata_scsiop_read_cap(struct ata_scsi_args *args, u8 *rbuf,
			        unsigned int buflen);
extern unsigned int ata_scsiop_report_luns(struct ata_scsi_args *args, u8 *rbuf,
				   unsigned int buflen);
extern void ata_scsi_badcmd(struct scsi_cmnd *cmd,
			    void (*done)(struct scsi_cmnd *),
			    u8 asc, u8 ascq);
extern void ata_scsi_set_sense(struct scsi_cmnd *cmd,
			       u8 sk, u8 asc, u8 ascq);
extern void ata_scsi_rbuf_fill(struct ata_scsi_args *args,
                        unsigned int (*actor) (struct ata_scsi_args *args,
                                           u8 *rbuf, unsigned int buflen));
extern void ata_schedule_scsi_eh(struct Scsi_Host *shost);
extern void ata_scsi_dev_rescan(void *data);
extern int ata_bus_probe(struct ata_port *ap);

/* libata-pmp.c */
extern int sata_pmp_scr_read(struct ata_link *link, int reg, u32 *val);
extern int sata_pmp_scr_write(struct ata_link *link, int reg, u32 val);
extern int sata_pmp_attach(struct ata_device *dev);

/* libata-eh.c */
extern enum scsi_eh_timer_return ata_scsi_timed_out(struct scsi_cmnd *cmd);
extern void ata_scsi_error(struct Scsi_Host *host);
extern void ata_port_wait_eh(struct ata_port *ap);
extern void ata_qc_schedule_eh(struct ata_queued_cmd *qc);
extern void ata_eh_detach_dev(struct ata_device *dev);
extern void ata_eh_about_to_do(struct ata_link *link, struct ata_device *dev,
			       unsigned int action);
extern void ata_eh_done(struct ata_link *link, struct ata_device *dev,
			unsigned int action);
extern void ata_eh_autopsy(struct ata_port *ap);
extern void ata_eh_report(struct ata_port *ap);
extern int ata_eh_reset(struct ata_link *link, int classify,
			ata_prereset_fn_t prereset, ata_reset_fn_t softreset,
			ata_reset_fn_t hardreset, ata_postreset_fn_t postreset);
extern int ata_eh_recover(struct ata_port *ap, ata_prereset_fn_t prereset,
			  ata_reset_fn_t softreset, ata_reset_fn_t hardreset,
			  ata_postreset_fn_t postreset,
			  struct ata_link **r_failed_disk);
extern void ata_eh_finish(struct ata_port *ap);
extern void ata_hp_poll_worker(void *data);
extern void ata_hp_poll_activate(struct ata_port *ap);
extern void ata_hp_poll_deactivate(struct ata_port *ap);

#endif /* __LIBATA_H__ */
