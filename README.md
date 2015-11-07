# Unholy Banking

Aqbanking wrapper plugin for nodejs

A list for supported banks: http://www.linuxwiki.de/OpenHBCI/GetesteteBanken

## Dependencies

Make sure you run the latest nodejs version:

    $ nodejs -v
    v4.2.1

You can compile and install gwenhywfar and aqbanking from source:

    $ export ACLOCAL=$(which aclocal) && export AUTOMAKE=$(which automake)
    $ git submodule update --init
    $ cd gwenhywfar \
    ./configure && make \
    sudo make install \
    cd ../aqbanking \
    ./configure && make
    $ sudo make install

or search and install it via package manager:

### Debian

    $ apt-get install libaqbanking-dev libgwenhywfar-dev

### Arch

    $ pacman -Sy aqbanking

## Build

    $ npm install
