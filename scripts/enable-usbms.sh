#!/bin/sh

# adapted to BQ Cervantes from Baskerville's Plato
# https://github.com/baskerville/plato/blob/master/scripts/usb-enable.sh
modprobe arcotg_udc

# BQ doesn't use the same settings in all devices
PCB_ID=$(/usr/bin/ntxinfo /dev/mmcblk0 | grep pcb | cut -d ":" -f2)
if [ "$PCB_ID" -eq 22 ] || [ "$PCB_ID" -eq 23 ]; then
    PRODUCT_ID=${PRODUCT_ID:-"0xAD78"}
    PARTITIONS="/dev/mmcblk0p7"
else
    PRODUCT_ID=${PRODUCT_ID:-"0xAD79"}
    PARTITIONS="/dev/mmcblk0p4"
fi

MODULE_PARAMETERS="vendor=0x2A47 product=${PRODUCT_ID} vendor_id=BQ product_id=Cervantes"

# share sdcard if present
[ -e /dev/mmcblk1p1 ] && PARTITIONS="${PARTITIONS},/dev/mmcblk1p1"

# write buffered data to disk.
sync

# free pagecache, dentries and inodes.
echo 3 >/proc/sys/vm/drop_caches

# umount partitions
for name in public sd; do
    DIR=/mnt/"$name"
    if grep -q "$DIR" /proc/mounts; then
        umount "$DIR" || umount -l "$DIR"
    fi
done

# shellcheck disable=SC2086
modprobe g_file_storage file=$PARTITIONS stall=1 removable=1 $MODULE_PARAMETERS
