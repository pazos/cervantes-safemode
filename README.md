# safe mode for BQ Cervantes devices

## Description

It lets you enable network over usb and telnet at boot pressing the home button.

It is not needed when running stock software but is handy if you're running 
third party software, like KOReader, as the only application on your ereader.

The application tries to catch home button press/release events for a few seconds.

## Usage

call this app from /etc/rc.local just before starting your application.

`/usr/bin/safemode && exit 1`

It can be used as a replacement of sleep in a loop, like:

while true; do
    /usr/bin/safemode && exit 1
    /mnt/private/koreader/koreader.sh
done

## How to enable safe mode once installed.

At boot you'll notice that the screen is cleared. Press the home button inmediatly.
You can tune the TIMEOUT to fit your needs

## Build from source

You need a working cross-compiler, like https://github.com/koreader/koxtoolchain, git
and debian tools to build packages.

```
git clone https://github.com/pazos/cervantes-safemode
cd cervantes-safemode
git submodule update --init --recursive
export CROSS_TC=arm-cervantes-linux-gnueabi
make
```

