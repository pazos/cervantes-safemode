ifdef CROSS_TC
        CC=$(CROSS_TC)-gcc
        STRIP=$(CROSS_TC)-strip
else
        CC?=gcc
        STRIP?=strip
endif

# All the warnings!
EXTRA_CFLAGS+=-Wall
EXTRA_CFLAGS+=-Wextra -Wunused
EXTRA_CFLAGS+=-Wformat=2
EXTRA_CFLAGS+=-Wformat-signedness
EXTRA_CFLAGS+=-Wformat-truncation=2
EXTRA_CFLAGS+=-Wnull-dereference
EXTRA_CFLAGS+=-Wuninitialized
EXTRA_CFLAGS+=-Wduplicated-branches -Wduplicated-cond
EXTRA_CFLAGS+=-Wundef
EXTRA_CFLAGS+=-Wbad-function-cast
EXTRA_CFLAGS+=-Wwrite-strings
EXTRA_CFLAGS+=-Wjump-misses-init
EXTRA_CFLAGS+=-Wlogical-op
EXTRA_CFLAGS+=-Wstrict-prototypes -Wold-style-definition
EXTRA_CFLAGS+=-Wshadow
EXTRA_CFLAGS+=-Wmissing-prototypes -Wmissing-declarations
EXTRA_CFLAGS+=-Wnested-externs
EXTRA_CFLAGS+=-Winline
EXTRA_CFLAGS+=-Wcast-qual
# NOTE: GCC 8 introduces -Wcast-align=strict to warn regardless of the target architecture (i.e., like clang)
EXTRA_CFLAGS+=-Wcast-align
EXTRA_CFLAGS+=-Wconversion
# And disable this, because it obviously doesn't play well with using goto to handle cleanup on error codepaths...
EXTRA_CFLAGS+=-Wno-jump-misses-init

PROG=safemode
SRCS=safemode.c

default: all

all: fbink safemode package

fbink:
	$(MAKE) -C FBInk cervantes

safemode:
	$(CC) $(CPPFLAGS) $(EXTRA_CPPFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) $(EXTRA_LDFLAGS) -LFBInk/Release -o$(PROG) $(SRCS) -l:libfbink.a -lm
	$(STRIP) --strip-unneeded $(PROG)

package:
	mkdir -p pkg/usr/bin
	cp safemode pkg/usr/bin
	mkdir -p pkg/usr/share/safemode/scripts
	cp scripts/*.sh pkg/usr/share/safemode/scripts
	mkdir -p pkg/usr/share/safemode/images
	cp images/*.png pkg/usr/share/safemode/images
	touch pkg/usr/share/safemode/version
	dpkg-deb -Z gzip -b pkg/ .

clean:
	$(MAKE) -C FBInk clean
	rm -rf safemode
	rm -rf pkg/usr
	rm -rf *.deb

.PHONY: default all fbink safemode package clean
