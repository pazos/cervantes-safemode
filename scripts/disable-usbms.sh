#!/bin/sh

# adapted to BQ Cervantes from Baskerville's Plato
# https://github.com/baskerville/plato/blob/master/scripts/usb-disable.sh

# BQ doesn't use the same partition scheme in all devices
PCB_ID=$(/usr/bin/ntxinfo /dev/mmcblk0 | grep pcb | cut -d ":" -f2)
if [ "$PCB_ID" -eq 22 ] || [ "$PCB_ID" -eq 23 ]; then
    PARTITION="/dev/mmcblk0p7"
else
    PARTITION="/dev/mmcblk0p4"
fi

# unload kernel modules
modprobe -r g_file_storage
sleep 1
modprobe -r arcotg_udc 2>/dev/null

# mount internal partition.
mount $PARTITION /mnt/public

# mount sdcard if present
if [ -e /dev/mmcblk1p1 ]; then
    mount /dev/mmcblk1p1 /mnt/sd
fi
