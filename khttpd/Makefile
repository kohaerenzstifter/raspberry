KOHAERENZSTIFTUNG := ../kohaerenzstiftung/

all: binary/httpd

binary/httpd: $(KOHAERENZSTIFTUNG)/Static/libkohaerenzstiftung.a
	cp $(KOHAERENZSTIFTUNG)/Static/libkohaerenzstiftung.a Release/
	cp $(KOHAERENZSTIFTUNG)/kohaerenzstiftung.h src/
	cd Release && make
	-mkdir -p binary
	cp Release/httpd binary/httpd

install: all
	install -m 755 binary/httpd /usr/bin/
	mkdir -p -m 755 /usr/lib/httpd
	mkdir -p -m 755 -p /usr/share/doc/khttpd
#	install -m 644 README /usr/share/doc/khttpd/
	install -m 644 khttpd.conf /etc/
	install -m 644 khttpd.logrotate /etc/logrotate.d/khttpd
	install -m 755 khttpd /etc/init.d/
#	update-rc.d khttpd defaults
#	service khttpd start

uninstall:
	service khttpd uninstall
	rm -rf /etc/logrotate.d/khttpd
	rm -rf /etc/khttpd.conf
	rm -rf /usr/bin/httpd
	rm -r /usr/lib/httpd
	rm -rf /usr/share/doc/khttpd

clean:
	cd Release && make clean
	rm -rf Release/libkohaerenzstiftung.a
	rm -rf src/kohaerenzstiftung.h
	rm -rf binary

