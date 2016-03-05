#!/bin/sh

checkinstall --pkgname=khttpd.infraradio --pkgsource=khttpd.infraradio \
	--maintainer=sancho@posteo.de --pkggroup=httpd --pkgversion=$1 \
	--pkgrelease=$2 --requires=khttpd,libglib2.0-0,lirc,wiringpi,libjson0 \
	--backup=no --install=no