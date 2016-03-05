#!/bin/sh

checkinstall --pkgname=khttpd.wwwidget --pkgsource=khttpd.wwwidget \
	--maintainer=sancho@posteo.de --pkggroup=httpd --pkgversion=$1 \
	--pkgrelease=$2 \
	--requires=khttpd,libglib2.0-0,libjson0,xvfb,cutycapt,imagemagick \
	--backup=no --install=no