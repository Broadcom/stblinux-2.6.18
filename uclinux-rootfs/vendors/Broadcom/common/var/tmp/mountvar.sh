#!/bin/bash

mountType=`grep mtdblock0 /proc/mounts | cut -f4 | cut -f1 -d','`

echo "mountType=$mountType"
case "$mountType" in
rw)
	exit
	;;
esac

if [ -f /tmpfsvar.img ]; then
	echo "mount -nt tmpfs -o size=512k,mode=777 /tmpfsvar.img /var"
	mount -nt tmpfs -o size=512k,mode=777 /tmpfsvar.img /var
	cd /var
	tar -xvf /var.img 1>/dev/null 2>/dev/null
fi

if [ -f /tmpfsetc.img ]; then
	echo "mount -nt tmpfs -o size=256k,mode=777 /tmpfsetc.img /etc"
	mount -nt tmpfs -o size=256k,mode=777 /tmpfsetc.img /etc
	cd /etc
	tar -xvf /etc.img 2>/dev/null 1>/dev/null
fi



