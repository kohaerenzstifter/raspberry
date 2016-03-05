#!/bin/sh

checkinstall --pkgname=khttpd --pkgsource=khttpd \
	--maintainer=sancho@posteo.de --pkggroup=httpd --pkgversion=$1 \
	--pkgrelease=$2 --requires=logrotate,libmicrohttpd10,libglib2.0-0 \
	--backup=no --install=no