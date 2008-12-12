/*
 * drivers/net/kgdboe.c
 *
 * A network interface for GDB.
 * Based upon 'gdbserial' by David Grothe <dave@gcom.com>
 * and Scott Foehner <sfoehner@engr.sgi.com>
 *
 * Maintainers: Amit S. Kale <amitkale@linsyssoft.com> and
 * 		Tom Rini <trini@kernel.crashing.org>
 *
 * 2004 (c) Amit S. Kale <amitkale@linsyssoft.com>
 * 2004-2005 (c) MontaVista Software, Inc.
 * 2005 (c) Wind River Systems, Inc.
 *
 * Other folks:
 * San Mehat <nettwerk@biodome.org>
 * Robert Walsh <rjwalsh@durables.org>
 * wangdi <wangdi@clusterfs.com>.
 * Matt Mackall <mpm@selenic.com>
 * Pavel Machek <pavel@suse.cz>
 * Jason Wessel <jason.wessel@windriver.com>
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 *
 * Changes:
 * 11/4/05 - Jason Wessel <jason.wessel@windriver.com>
 * - Added re-configration via sysfs
 *
 * 3/10/05 - Jason Wessel <jason.wessel@windriver.com>
 * - Added ability to compile/load as module
 *
 * Known problems:
 * - There is no way to deny the unloading of the module
 *   if KGDB is connected, but an attempt is made to handle it
 */

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/kgdb.h>
#include <linux/netpoll.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <asm/atomic.h>

#define MAX_KGDBOE_CONFIG_STR 256
#define NOT_CONFIGURED_STRING "not_configured"
#define IN_BUF_SIZE 512		/* power of 2, please */
#define OUT_BUF_SIZE 30		/* We don't want to send too big of a packet. */

static char in_buf[IN_BUF_SIZE], out_buf[OUT_BUF_SIZE];
static int in_head, in_tail, out_count;
static atomic_t in_count;
/* 0 = unconfigured, 1 = netpoll options parsed, 2 = fully configured. */
static int configured = 0;
static int use_dynamic_mac = 0;

static void rx_hook(struct netpoll *np, int port,
		    char *msg, int len, struct sk_buff *skb);
static void eth_pre_exception_handler(void);
static void eth_post_exception_handler(void);
static int eth_get_char(void);
static void eth_flush_buf(void);
static void eth_put_char(int);
int init_kgdboe(void);
static int param_set_kgdboe_var(const char *kmessage, struct kernel_param *kp);

#define LOCAL_PORT 6443
#define REMOTE_PORT 6442
#define LOCAL_DEV_NAME "eth0"

static struct netpoll np = {
	.name = "kgdboe",
	.dev_name = LOCAL_DEV_NAME,
	.rx_hook = rx_hook,
	.local_port = LOCAL_PORT,
	.remote_port = REMOTE_PORT,
	.remote_mac = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
};

MODULE_DESCRIPTION("KGDB driver for network interfaces");
MODULE_LICENSE("GPL");
char config[MAX_KGDBOE_CONFIG_STR] = NOT_CONFIGURED_STRING;
static struct kparam_string kps = {
	.string = config,
	.maxlen = MAX_KGDBOE_CONFIG_STR - 1
};

module_param_call(kgdboe, param_set_kgdboe_var, param_get_string, &kps, 0644);
MODULE_PARM_DESC(kgdboe,
		 " kgdboe=[src-port]@[src-ip]/[dev],[tgt-port]@<tgt-ip>/<tgt-macaddr>\n");

#ifdef CONFIG_KGDBOE_MODULE
/* If it is a module the kgdb_io_ops should be a static which
 * is passed to the KGDB I/O initialization
 */
static struct kgdb_io local_kgdb_io_ops = {
#else				/* ! CONFIG_KGDBOE_MODULE */
struct kgdb_io kgdb_io_ops = {
#endif				/* ! CONFIG_KGDBOE_MODULE */
	.read_char = eth_get_char,
	.write_char = eth_put_char,
	.init = init_kgdboe,
	.flush = eth_flush_buf,
	.late_init = NULL,
	.pre_exception = eth_pre_exception_handler,
	.post_exception = eth_post_exception_handler
};

#ifdef CONFIG_KGDBOE_MODULE
static struct kgdb_io *my_kgdb_io_ops = &local_kgdb_io_ops;
#else				/* ! CONFIG_KGDBOE_MODULE */
static struct kgdb_io *my_kgdb_io_ops = &kgdb_io_ops;
#endif				/* ! CONFIG_KGDBOE_MODULE */

static void use_dynamic_mac_check(struct netpoll *np)
{
	/* If check if the MAC address is all 1's and if so use dynamic
	 * MAC address assigment so we don't generate broadcast
	 * traffic. 
	 */
	if (np->remote_mac[0] == 0xff &&
	    np->remote_mac[1] == 0xff &&
	    np->remote_mac[2] == 0xff &&
	    np->remote_mac[3] == 0xff &&
	    np->remote_mac[4] == 0xff && np->remote_mac[5] == 0xff) {
		use_dynamic_mac = 1;
	} else {
		use_dynamic_mac = 0;
	}
}

static int param_set_kgdboe_var(const char *kmessage, struct kernel_param *kp)
{
	struct kparam_string *kps = kp->arg;
	char kmessage_save[MAX_KGDBOE_CONFIG_STR];
	char kmessage_chopped[MAX_KGDBOE_CONFIG_STR];
	int msg_len = strlen(kmessage);

	if (msg_len + 1 > kps->maxlen) {
		printk(KERN_ERR "%s: string doesn't fit in %u chars.\n",
		       kp->name, kps->maxlen - 1);
		return -ENOSPC;
	}
	if (configured == 0) {
		/* If we have not run through the init routine yet, we should not
		 * do any sort of parsing and simply set the string passed by the
		 * caller.
		 */
		strcpy(kps->string, kmessage);
		return 0;
	}

	/* This is considered a runtime change because the module or
	 * built-in kgdboe is being reconfigured 
	 */
	strncpy(kmessage_chopped, kmessage, sizeof(kmessage_chopped));
	msg_len--;
	while (msg_len > 0 &&
	       (kmessage_chopped[msg_len] == '\n' ||
		kmessage_chopped[msg_len] == '\r' ||
		kmessage_chopped[msg_len] == ' ' ||
		kmessage_chopped[msg_len] == '\t')) {
		kmessage_chopped[msg_len] = '\0';
		msg_len--;
	}
	/* Check to see if we are unconfiguring the io module */
	if (configured == 2
	    && strcmp(kmessage_chopped, NOT_CONFIGURED_STRING) == 0) {
		printk(KERN_INFO "kgdboe: reverting to unconfigured state\n");
		netpoll_cleanup(&np);
		kgdb_unregister_io_module(my_kgdb_io_ops);
		/* If we are the built-in zero out the kgdb_io_ops */
		if (my_kgdb_io_ops == &kgdb_io_ops)
			memset(&kgdb_io_ops, 0, sizeof(struct kgdb_io));
		strcpy(kps->string, NOT_CONFIGURED_STRING);
		configured = 1;
		return 0;
	}

	/* Setup the orignal values to the np structure */
	strcpy(np.dev_name, LOCAL_DEV_NAME);
	np.local_port = LOCAL_PORT;
	np.remote_port = REMOTE_PORT;
	np.remote_mac[0] = 0xff;
	np.remote_mac[1] = 0xff;
	np.remote_mac[2] = 0xff;
	np.remote_mac[3] = 0xff;
	np.remote_mac[4] = 0xff;
	np.remote_mac[5] = 0xff;

	/* Try out the new net poll settings.  We have to use a saved
	 * buffer because the netpoll_parse_options can destroy parts of
	 * the buffer.
	 */
	strncpy(kmessage_save, kmessage_chopped, sizeof(kmessage_save));

	if (netpoll_parse_options(&np, kmessage_save) != 0) {
		printk(KERN_ERR
		       "kgdboe: Usage: [src-port]@[src-ip]/[dev],[tgt-port]@<tgt-ip>/[tgt-macaddr]\n");
		if (strncmp
		    (kps->string, NOT_CONFIGURED_STRING,
		     sizeof(kps->string)) != 0) {
			printk(KERN_ERR
			       "kgdboe: reverting to prior configuration\n");
			/* revert back to the original config */
			strncpy(kmessage_save, kps->string,
				sizeof(kmessage_save));
			netpoll_parse_options(&np, kmessage_save);
			/* After each parse we check the use of the dynamic mac feature */
			use_dynamic_mac_check(&np);
		}
		return 0;
	}

	/* After each parse we check the use of the dynamic mac feature */
	use_dynamic_mac_check(&np);

	/* The parse worked so, save the config string in the internal
	 * static variable 
	 */
	strcpy(kps->string, kmessage_chopped);

	/* Bail unless we are are in the partially configured state. */
	if (configured != 1) {
		return 0;
	}
	if (!
	    (kgdb_io_ops.init == NULL
	     || kgdb_io_ops.init == my_kgdb_io_ops->init)) {
		printk(KERN_ERR
		       "kgdboe: is not active debug module. It cannot be re-configured\n");
		return 0;
	}
	/* We are in the partially configured state, and we attempt to
	 * complete the configuration. Do the net poll setup at this time
	 */
	if (netpoll_setup(&np)) {
		printk(KERN_ERR "kgdboe: netpoll_setup failed kgdboe failed\n");
		return 0;
	}

	/* This takes care of a module or a built in */
	my_kgdb_io_ops->read_char = eth_get_char;
	my_kgdb_io_ops->write_char = eth_put_char;
	my_kgdb_io_ops->init = init_kgdboe;
	my_kgdb_io_ops->flush = eth_flush_buf;
	my_kgdb_io_ops->late_init = NULL;
	my_kgdb_io_ops->pre_exception = eth_pre_exception_handler;
	my_kgdb_io_ops->post_exception = eth_post_exception_handler;

	if (kgdb_register_io_module(my_kgdb_io_ops)) {
		netpoll_cleanup(&np);
		/* If we are the built-in zero out the kgdb_io_ops */
		if (my_kgdb_io_ops == &kgdb_io_ops)
			memset(&kgdb_io_ops, 0, sizeof(struct kgdb_io));
	} else
		printk(KERN_INFO "kgdboe: debugging over ethernet enabled\n");
	configured = 2;

	return 0;
}

static void eth_pre_exception_handler(void)
{
	netpoll_set_trap(1);
}

static void eth_post_exception_handler(void)
{
	netpoll_set_trap(0);
}

static int eth_get_char(void)
{
	int chr;


//	printk(KERN_INFO "kgdboe: eth_get_char\n");

	while (atomic_read(&in_count) == 0)
		netpoll_poll(&np);


	chr = in_buf[in_tail++];
	in_tail &= (IN_BUF_SIZE - 1);
	atomic_dec(&in_count);

#ifdef DEBUG_KGDBOE 
	printk(KERN_INFO "kgdboe: eth_et_char got:%c \n", chr);
#endif

	return chr;
}

static void eth_flush_buf(void)
{
	if (out_count && np.dev) {
		netpoll_send_udp(&np, out_buf, out_count);
		memset(out_buf, 0, sizeof(out_buf));
		out_count = 0;
	}
}

static void eth_put_char(int chr)
{
#ifdef DEBUG_KGDBOE 
	printk(KERN_INFO "kgdboe: eth_put_char got:%c \n", chr);
#endif

	out_buf[out_count++] = chr;
	if (out_count == OUT_BUF_SIZE)
		eth_flush_buf();
}

static void rx_hook(struct netpoll *np, int port, char *msg,
		    int len, struct sk_buff *skb)
{
	int i;

	np->remote_port = port;

	/*
	 * This could be GDB trying to attach.  But it could also be GDB
	 * finishing up a session, with kgdb_connected=0 but GDB sending
	 * an ACK for the final packet.  To make sure we don't try and
	 * make a breakpoint when GDB is leaving, make sure that if
	 * !kgdb_connected the only len == 1 packet we allow is ^C.
	 */
	if (!kgdb_connected && (len != 1 || msg[0] == 3) 
        /* && !atomic_read(&kgdb_setting_breakpoint) */) {
#if 0//vincent
		if (use_dynamic_mac) {
			memcpy(np->remote_mac, eth_hdr(skb)->h_source,
			       sizeof(np->remote_mac));
		}
#endif //0
#ifdef DEBUG_KGDBOE 
		printk(KERN_INFO "rx_hook() 1 .................................. \r\n");
#endif

		tasklet_schedule(&kgdb_tasklet_breakpoint);
	}

	for (i = 0; i < len; i++) {
		if (msg[i] == 3) { 

#ifdef DEBUG_KGDBOE 
		printk(KERN_INFO "rx_hook() 2 .................................. \r\n");
#endif

			tasklet_schedule(&kgdb_tasklet_breakpoint);
		}

		if (atomic_read(&in_count) >= IN_BUF_SIZE) {
			/* buffer overflow, clear it */
			in_head = in_tail = 0;
			atomic_set(&in_count, 0);
			break;
		}
		in_buf[in_head++] = msg[i];
		in_head &= (IN_BUF_SIZE - 1);
		atomic_inc(&in_count);
	}
}

/* We must be passed configuration options. */
static int option_setup(char *opt)
{
	char temp_opt[MAX_KGDBOE_CONFIG_STR];
	/* The net poll parser can destroy the string so use a dummy copy
	 * off the stack 
	 */
	strncpy(temp_opt, opt, sizeof(temp_opt));
	configured = !netpoll_parse_options(&np, temp_opt);
	/* After each parse we check the use of the dynamic mac feature */
	use_dynamic_mac_check(&np);

	return 0;
}

__setup("kgdboe=", option_setup);

int init_kgdboe(void)
{
	/* Already done? */
	if (configured == 2)
		return 0;

	if (strlen(config))
		option_setup(config);

	if (!configured) {
		/* When we are a built in, we must set the configured to 1 because
		 * the init routine has run at this point.  This will alow
		 * reconfiguration later on durring runtime.
		 */
		configured = 1;
		printk(KERN_ERR
		       "kgdboe: configuration incorrect - kgdboe not loaded.\n");
		printk(KERN_ERR
		       "  Usage: kgdboe=[src-port]@[src-ip]/[dev],[tgt-port]@<tgt-ip>/[tgt-macaddr]\n");
		return -EINVAL;
	}

	/* We have made it far enough to consider we are partially
	 * configured becase the init routine has fired.  The rest can be
	 * taken care of at run time.
	 */
	configured = 1;
	if (netpoll_setup(&np)) {
		printk(KERN_ERR "kgdboe: netpoll_setup failed kgdboe failed\n");
		return -EINVAL;
	}

	if (kgdb_register_io_module(my_kgdb_io_ops)) {
		netpoll_cleanup(&np);
		return -EINVAL;
	}

	printk(KERN_INFO "kgdboe: debugging over ethernet enabled\n");

	configured = 2;


	return 0;
}

static void cleanup_kgdboe(void)
{
	netpoll_cleanup(&np);
	configured = 0;

	kgdb_unregister_io_module(my_kgdb_io_ops);
}

module_init(init_kgdboe);
module_exit(cleanup_kgdboe);
