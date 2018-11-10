ifdef CROSS_TC
        CC=$(CROSS_TC)-gcc
        STRIP=$(CROSS_TC)-strip
else
        CC?=gcc
        STRIP?=strip
endif


PROG=safemode
SRCS=safemode.c

default: all

all: fbink safemode package

fbink:
	cd FBInk && \
	$(MAKE) cervantes strip

safemode:
	$(CC) $(SRCS) -LFBInk/Release -l:libfbink.a -lm -o $(PROG)
	$(STRIP) $(PROG)

package:
	mkdir -p pkg/usr/bin
	cp safemode pkg/usr/bin
	mkdir -p pkg/usr/share/safemode/scripts
	cp scripts/*.sh pkg/usr/share/safemode/scripts
	mkdir -p pkg/usr/share/safemode/images
	cp images/*.png pkg/usr/share/safemode/images

	dpkg-deb -b pkg/ .

clean:
	cd FBInk && \
	$(MAKE) clean
	rm -rf safemode
	rm -rf pkg/usr
	rm -rf *.deb

.PHONY: default all fbink safemode package clean
