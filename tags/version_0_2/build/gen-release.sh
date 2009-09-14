#!/bin/sh

D=`pwd`

VER="0.2"

rm -fr linux-install
mkdir -p linux-install

./gen.sh && echo "Generated build files .." && \
cd linux && make install DESTDIR="$D/linux-install" && echo "Installed source." && \
cd .. && mv linux-install bdremote-ng-$VER && tar cvjf bdremote-ng-$VER.tar.bz2 bdremote-ng-$VER
