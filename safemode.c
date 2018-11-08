/* safemode.c - turn on usbnet at boot on BQ Cervantes based on button events.
   Copyright (C) 2018 Martín Fernández <paziusss@gmail.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include "FBInk/fbink.h"

// time to wait for events, in milliseconds
#define TIMEOUT 2000

// event code for BQ Cervantes home button
#define BTN_HOME 61

// keypad present on Kobos & Cervantes
#define EB600_KEYPAD "/dev/input/event0"

// usbnet config (from /usr/bin/usbup.sh)
#define IP_ADDR	"192.168.4.1"
#define NETMASK	"255.255.255.0"
#define MAC_ADDR "2a:0f:11:21:85:df"

// use FBInk to write stuff to the screen.
FBInkConfig fbink_config = { 0 };

static void
    init_fbink_config(void)
{
    fbink_config.row 		= 8;
    fbink_config.is_centered 	= true;
    fbink_config.is_cleared	= true;
    fbink_config.is_padded	= true;
    fbink_config.is_quiet	= true;
}

// returns SUCESS if safemode was enabled, FAILURE otherwise.
int main(void)
{
    int fd;
    int retval = 0;
    int counter = 0;
    char command[80];
    struct input_event event;

    // Open input device.
    if ((fd = open(EB600_KEYPAD, O_RDONLY | O_NONBLOCK)) == -1)
    {
        fprintf(stderr, "Failed to open %s, aborting!\n", EB600_KEYPAD);
        exit(EXIT_FAILURE);
    }

    // Initialize fbink
    init_fbink_config();
    if (fbink_init(FBFD_AUTO, &fbink_config) != EXIT_SUCCESS)
    {
	fprintf(stderr, "Failed to initialize FBInk, aborting!\n");
        exit(EXIT_FAILURE);
    }

    // Clear the screen as visual feedback before starting the loop
    fbink_printf(FBFD_AUTO, NULL, &fbink_config, "");

    // Main loop waiting for home button events.
    fprintf(stdout, "Waiting for home key during %d seconds ...\n", TIMEOUT/1000);
    while (1)
    {
        // we don't care about event.value here, so we catch both press and release events.
        if ((read(fd, &event, sizeof(struct input_event)) != -1) && (event.code == BTN_HOME)) {
            retval = 1;
            break;
        }

        // counter is meassured in milliseconds, usleep uses microseconds.
        counter++;
        usleep(1000);

        // break the loop if we reach the timeout.
        if (counter > TIMEOUT) {
            break;
        }
    }

    close(fd);
    if (retval == 1) {
        // Use safemode (usbnet)
        fbink_config.is_cleared = false;
        fbink_printf(FBFD_AUTO, NULL, &fbink_config, "starting usbnet");
        fprintf(stdout, "home key pressed -> starting usbnet\n");

        // print usbnet config on screen
        fbink_config.row = fbink_config.row + 2;
        fbink_printf(FBFD_AUTO, NULL, &fbink_config, "ip addr: %s", IP_ADDR);
        fbink_config.row++;
        fbink_printf(FBFD_AUTO, NULL, &fbink_config, "netmask: %s", NETMASK);
        fbink_config.row++;
        fbink_printf(FBFD_AUTO, NULL, &fbink_config, "mac addr: %s", MAC_ADDR);

        // load kernel modules
        sprintf(command, "modprobe arcotg_udc\n");
        system(command);
        sprintf(command, "modprobe g_ether host_addr=%s\n", MAC_ADDR);
        system(command);

        // bring up network interface
        sprintf(command, "ifconfig usb0 inet %s netmask %s\n", IP_ADDR, NETMASK);
        system(command);

        // start telnet via inet daemon
        sprintf(command, "/usr/sbin/inetd");
        system(command);

        // exit with error means stopping /etc/rc.local execution before starting
        // the main application and it is what we want here.
        exit(EXIT_SUCCESS);
    } else {
        exit(EXIT_FAILURE);
    }
}
