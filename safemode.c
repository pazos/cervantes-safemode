#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* use FBInk to print stuff on screen */
#include "FBInk/fbink.h"
FBInkConfig fb = { 0 };

/* input events */
#include <linux/input.h>
#define KEYPAD "/dev/input/event0"

/* wait for events timeout */
#define TIMEOUT 2000

int
  main(int argc, char** argv)
{
    int                fd, ret;
    int                count = 0;
    int                steps = TIMEOUT / 10;
    struct input_event ev;
    enum usb_modes
    {
        usbnet,
        usbms
    } mode;

    // read mode from first argument
    if ((argc > 1) && (strcmp(argv[1], "network") == 0)) {
        mode = usbnet;
    } else if ((argc > 1) && (strcmp(argv[1], "storage") == 0)) {
        mode = usbms;
    } else {
        fprintf(stderr, "Usage: %s [network|storage]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // open input device.
    if ((fd = open(KEYPAD, O_RDONLY | O_NONBLOCK)) == -1) {
        fprintf(stderr, "Failed to open %s, aborting!\n", KEYPAD);
        exit(EXIT_FAILURE);
    }

    // initialize FBInk
    if (fbink_init(FBFD_AUTO, &fb) != EXIT_SUCCESS) {
        fprintf(stderr, "Failed to initialize FBInk, aborting!\n");
        exit(EXIT_FAILURE);
    }

    // we can force-entering a mode and skip button presses
    if ((argc > 2) && (strcmp(argv[2], "--force") == 0)) {
        ret = 1;
    } else {
        // or we can start a loop waiting for events instead..
        fprintf(stdout, "Waiting for home button press during %d seconds ...\n", TIMEOUT / 1000);
        while (1) {
            if (count == 0) {
                fb.row         = -2;
                fb.is_centered = true;
                fbink_printf(FBFD_AUTO, NULL, &fb, ".");
            } else if (count == steps * 2) {
                fbink_printf(FBFD_AUTO, NULL, &fb, "..");
            } else if (count == steps * 4) {
                fbink_printf(FBFD_AUTO, NULL, &fb, "...");
            } else if (count == steps * 6) {
                fbink_printf(FBFD_AUTO, NULL, &fb, "....");
            } else if (count == steps * 8) {
                fbink_printf(FBFD_AUTO, NULL, &fb, ".....");
            } else if (count == TIMEOUT) {
                fbink_printf(FBFD_AUTO, NULL, &fb, "     ");
                ret = 0;
                break;
            }
            // break on home button press event
            if ((read(fd, &ev, sizeof(struct input_event)) != -1) && (ev.code == 61) && (ev.value == 1)) {
                ret = 1;
                break;
            }
            count++;
            usleep(1000);    // microseconds
        }
    }

    if (ret == 1) {
        // start usb gadget
        fprintf(stdout, "starting %s mode, press the button to stop it\n", argv[1]);
        fb.row         = 0;
        fb.halign      = CENTER;
        fb.valign      = CENTER;
        fb.is_cleared  = true;
        fb.is_flashing = true;
        if (mode == usbnet) {
            fbink_print_image(FBFD_AUTO, "/usr/share/safemode/images/usbnet.png", 0, 0, &fb);
            system("/usr/share/safemode/scripts/enable-usbnet.sh\n");
        } else if (mode == usbms) {
            fbink_print_image(FBFD_AUTO, "/usr/share/safemode/images/usbms.png", 0, 0, &fb);
            system("/usr/share/safemode/scripts/enable-usbms.sh\n");
        }

        // wait for another home button press event, this time without timeout.
        while (1) {
            usleep(1000);
            // break on home button press event
            if ((read(fd, &ev, sizeof(struct input_event)) != -1) && (ev.code == 61) && (ev.value == 1)) {
                break;
            }
        }
        // stop usb gadget
        fprintf(stdout, "button pressed, stopping %s mode\n", argv[1]);
        if (mode == usbnet) {
            system("/usr/share/safemode/scripts/disable-usbnet.sh\n");
        } else if (mode == usbms) {
            system("/usr/share/safemode/scripts/disable-usbms.sh\n");
        }
        close(fd);
        fbink_printf(FBFD_AUTO, NULL, &fb, " ");
        exit(EXIT_SUCCESS);
    } else {
        // timeout without event, nothing to do!
        close(fd);
        exit(EXIT_SUCCESS);
    }
}
