/*
 * Copyright (c) 2002-2008 Broadcom Corporation 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
*/
/* uniMac proc file system support */
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <asm/brcmstb/common/brcmstb.h>
#include "bcmunimac.h"
#include "unimac_proc.h"

static int proc_ntos(void *buf, int count, const char * format, unsigned long value);
static int proc_ston(const void *buf, int count, unsigned long *value);
/*
 * utility find base register based on proc group
 */
static inline volatile unsigned long * bcmumac_proc2base(const BcmEnet_devctrl * pDevCtrl, const umacProcEntry * entry)
{
	switch(entry->group) {
		case PROC_GROUP_UMAC:
			return (volatile unsigned long*)pDevCtrl->umac;
		case PROC_GROUP_RBUF:
			return (volatile unsigned long*)pDevCtrl->txrx_ctrl;
		case PROC_GROUP_INTRL2:
			return (volatile unsigned long*)pDevCtrl->intrl2;
		case PROC_GROUP_HFB:
			return (volatile unsigned long*)pDevCtrl->hfb;
		case PROC_GROUP_ISDMA:
			return (volatile unsigned long*)pDevCtrl->dmaRegs;
		case PROC_GROUP_EPLL:
			return (volatile unsigned long*)pDevCtrl->epll;
		default:
			return 0;
	}
}
/*
 * proc entry read function.
 * used for reading single register or register bit/s.
 */
static int bcmumac_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	volatile unsigned long * reg;
	BcmEnet_devctrl * pDevCtrl;
	volatile unsigned long * base;
	umacProcEntry * entry = (umacProcEntry *)data;
	
	/* If the entry has a "display" function, display it and return. */
	if (entry->fn != NULL) {
		entry->fn(page, (unsigned long*)entry->context);
		*eof = 1;
		return 0;
	}
	pDevCtrl = (BcmEnet_devctrl *)entry->context;
	base = bcmumac_proc2base(pDevCtrl, entry);

	if(!base) {
		*eof = 1;
		return -EINVAL;
	}
	reg = base + entry->offset;
	proc_ntos(page, count, "%u\n", (DEV_RD(reg) >> entry->shift) & entry->mask);

	*eof = 1;
	return 0;
}
/*
 * proc write function.
 * used for writing single register or register bit/s
 */
static int bcmumac_proc_write(struct file *file, const char * buffer, unsigned long count, void * data)
{
	umacProcEntry * entry = (umacProcEntry *)data;
	BcmEnet_devctrl * pDevCtrl = (BcmEnet_devctrl *)entry->context;
	volatile unsigned long * base = bcmumac_proc2base(pDevCtrl, entry);
	volatile unsigned long * reg;
	unsigned long old, new;

	if(!base) {
		return -EINVAL;
	}
	reg = base + entry->offset;
	if(proc_ston(buffer, count, &new) >= 0) {
		old = DEV_RD(reg);
		old &= ~(entry->mask << entry->shift);
		old |= ((new & entry->mask ) << entry->shift);
		DEV_WR(reg, old);
		return 0;
	}
	return -EINVAL;
}
/*
 * proc support function.
 */
static int proc_entry_parse(char* name, int* argc, char* argv[])
{
	int result = 0;

	while (*name != '\0')
	{
		if (*name == '/')
		{
			if (result == *argc) return -ENOMEM;

			*name++ = '\0';
			argv[result++] = name;
		}

		++name;
	}

	return (*argc = result);
}

static struct proc_dir_entry* proc_entry_find(struct proc_dir_entry* dir_entry, char* name)
{
	struct proc_dir_entry* sub_entry;

	for (	sub_entry = dir_entry->subdir;
			sub_entry != NULL;
			sub_entry = sub_entry->next	)
	{
		if (strcmp(sub_entry->name, name) == 0) break;
	}

	return sub_entry;
}

static struct proc_dir_entry* proc_entry_update(struct proc_dir_entry* dir_entry, int argc, char* argv[])
{
	struct proc_dir_entry* sub_entry = proc_entry_find(dir_entry, argv[0]);

	if (sub_entry == NULL)
	{
		sub_entry = ((argc != 1)
						? proc_mkdir(argv[0], dir_entry)
						: create_proc_entry(argv[0], 0, dir_entry));
	}

	if ((sub_entry != NULL) && (argc != 1))
	{
		sub_entry = proc_entry_update(sub_entry, argc-1, argv+1);
	}

	return sub_entry;
}

static void proc_entry_delete(struct proc_dir_entry* dir_entry, int argc, char* argv[])
{
	struct proc_dir_entry* sub_entry = proc_entry_find(dir_entry, argv[0]);

	if (sub_entry != NULL)
	{
		if (argc != 1) proc_entry_delete(sub_entry, argc-1, argv+1);

		if (sub_entry->subdir == NULL)
		{
			remove_proc_entry(argv[0], dir_entry);
		}
	}
}

/* create proce entry */
int bcmumac_proc_entry_create(char *path, umacProcEntry * entry)
{
	struct proc_dir_entry * dir_entry = 0;
	char name[128];
	int argc = 16;
	char *argv[16];

	strcpy(name, path);
	strcat(name, entry->name);

	/* Create directory or sub-dir if the name has '/' in it*/
	if( proc_entry_parse(name, &argc, argv) >=0) {
		dir_entry = proc_entry_update(&proc_root, argc-1, argv+1);
		if(dir_entry != NULL)
		{
			dir_entry->mode = entry->mode;
			if(dir_entry->mode & S_IRUSR || 
					dir_entry->mode & S_IRGRP ||
					dir_entry->mode & S_IROTH) 
			{
				dir_entry->read_proc = bcmumac_proc_read;
			}else if(dir_entry->mode & S_IWUSR ||
					dir_entry->mode & S_IWGRP ||
					dir_entry->mode & S_IWOTH)
			{
				dir_entry->read_proc = bcmumac_proc_read;
				dir_entry->write_proc = bcmumac_proc_write;
			}
		}
		dir_entry->data = entry;
	}
	return (dir_entry ? 0: -ENODEV);
}
/* remove a proce entry*/
int bcmumac_proc_entry_remove(char *path, umacProcEntry * entry)
{
	char name[128];
	int argc = 16;
	char *argv[16];

	strcpy(name, path);
	strcat(name, entry->name);
	if(proc_entry_parse(name, &argc, argv) >= 0)
	{
		proc_entry_delete(&proc_root, argc-1, argv+1);
	}
	return 0;
}
/* convert number to string.*/
static int proc_ntos(void *buf, int count, const char * format, unsigned long value)
{
	return sprintf(buf, format, value);
}
/* convert string to number */
static int proc_ston(const void *buf, int count, unsigned long *value)
{
	char *last = "INVALID";
	char *arg = (char*)buf;

	if(arg[0] == '0')
	{
		if((arg[1] == 'x') || (arg[1] == 'X'))
		{
			*value = simple_strtoul(arg+2, &last, 16);
		}else {
			*value = simple_strtoul(arg+1, &last, 8);
		}
	}else {
		*value = simple_strtoul(arg, &last, 10);
	}
	return (((*last == '\0') || (*last == '\0')) ? 0: (-EINVAL));
			
}
