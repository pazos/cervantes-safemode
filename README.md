# safe mode for Cervantes ereaders

## Description

Enable network or storage over usb. 

Unneeded if you're running stock, it can be useful with thirdparty software

## Usage

`safemode [network|storage]`

Call this tool from /etc/rc.local  to provide an early usbnet connection, even before starting the main application:
`safemode network`

Use it inside loops:
```
while true; do
    safemode storage
    /mnt/private/koreader/koreader.sh
done
```
You can tune the TIMEOUT constant to fit your needs.


## How to enter/leave safemode once installed.

You'll see little dots at the bottom of the screen, press the home button before the dots are cleared to start.
Press the button again to stop.

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

## Thanks

To @NiLuJe for [FBInk](https://github.com/NiLuJe/FBInk) and to BQ for [fulfill Qt opensource license](https://github.com/search?q=org%3Abq+cervantes&unscoped_q=cervantes)
