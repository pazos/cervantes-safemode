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
	$(MAKE) cervantes strip MINIMAL=true

safemode:
	$(CC) $(SRCS) -LFBInk/Release -l:libfbink.a -o $(PROG)
	$(STRIP) $(PROG)

package:
	mkdir -p pkg/usr/bin
	cp safemode pkg/usr/bin
	dpkg-deb -b pkg/ .

clean:
	cd FBInk && \
	$(MAKE) clean
	rm -rf safemode
	rm -rf pkg/usr
	rm -rf *.deb

.PHONY: default all fbink safemode package clean
