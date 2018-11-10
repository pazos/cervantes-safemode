#!/bin/sh

# bring down network interface
ifconfig usb0 down

# disable telnet if running
kill -9 "$(pidof inetd)" 2>/dev/null

# unload kernel modules
modprobe -r g_ether
sleep 1
modprobe -r arcotg_udc 2>/dev/null
