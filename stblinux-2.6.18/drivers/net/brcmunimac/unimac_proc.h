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
#ifndef UNIMAC_PROC_H
#define UNIMAC_PROC_H   
/*
 * proc file support header 
 */
#define	PROC_GROUP_UMAC		0
#define PROC_GROUP_RBUF		1
#define PROC_GROUP_INTRL2	2
#define PROC_GROUP_HFB		3
#define PROC_GROUP_ISDMA	4
#define PROC_GROUP_EPLL		6

/* Function pointer to decode a block of memory for debug.*/
typedef int (*display_fn)( char * buf, unsigned long * addr);
/* proc entry */
typedef struct umac_proc_t
{
	void * context;		/* context, e.g. a device handle, or start address of a block of memory to decode */
	display_fn fn;		/* function to display a block of memory */
	int group;			/* proc groups, umac/intrl2/rbuf/isdma....*/
	int mode;			/* read/write permission */
	int offset;			/* Register offset from base register */
	int shift;			/* bit shift */
	int mask;			/* bit mask */
	char *name;			/* proc entry name*/
}umacProcEntry;

int bcmumac_proc_entry_create(char *path, umacProcEntry * entry);
int bcmumac_proc_entry_remove(char *path, umacProcEntry * entry);

/* script generated proc entries */
extern umacProcEntry umac0_proc_entries[234];
extern umacProcEntry umac1_proc_entries[234];

#endif /* UNIMAC_PROC_H */
