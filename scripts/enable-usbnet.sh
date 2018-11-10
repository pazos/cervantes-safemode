#!/bin/sh

# keep it compatible with /usr/bin/usbup.sh
MAC_ADDR="2a:0f:11:21:85:df"
IP_ADDR="192.168.4.1"
NETMASK="255.255.255.0"

# load kernel modules
modprobe arcotg_udc
modprobe g_ether host_addr="$MAC_ADDR"

# setup usb network interface
ifconfig usb0 inet "$IP_ADDR" netmask "$NETMASK"

# start telnetd
/usr/sbin/inetd
