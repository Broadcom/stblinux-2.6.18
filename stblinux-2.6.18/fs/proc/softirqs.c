#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static const char *desc_array[] = {
	"HI",
	"TIMER",
	"NET_TX",
	"NET_RX",
	"BLOCK",
	"TASKLET",
	"SCHED",
#ifdef CONFIG_HIGH_RES_TIMERS
	"HRTIMER",
#endif
	"RCU"};

/*
 * /proc/softirqs  ... display the number of softirqs
 */
static int show_softirqs(struct seq_file *p, void *v)
{
	int i, j;

	seq_printf(p, "                ");
	for_each_online_cpu(i)
		seq_printf(p, "CPU%-8d", i);
	seq_printf(p, "\n");

	for_each_softirq_nr(i) {
		seq_printf(p, "%-10s", desc_array[i]);
		for_each_online_cpu(j)
			seq_printf(p, "%10u ", kstat_softirqs_cpu(i, j));
		seq_printf(p, "\n");
	}
	return 0;
}

static int softirqs_open(struct inode *inode, struct file *file)
{
	return single_open(file, show_softirqs, NULL);
}

static struct file_operations proc_softirqs_operations = {
	.open		= softirqs_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init proc_softirqs_init(void)
{
	create_seq_entry("softirqs", 0, &proc_softirqs_operations);
	return 0;
}
module_init(proc_softirqs_init);
