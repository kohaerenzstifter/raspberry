KOHAERENZSTIFTUNG := ../kohaerenzstiftung/
KHTTPD := ../khttpd/

all: binary/libhttpdWwwidget.so

binary/libhttpdWwwidget.so:
	cp $(KOHAERENZSTIFTUNG)/kohaerenzstiftung.h .
	cp $(KHTTPD)/src/httpd.h .
	cd Release && make
	-mkdir -p binary
	cp Release/libhttpdWwwidget.so binary/libhttpdWwwidget.so

install: all
	install -m 644 binary/libhttpdWwwidget.so /usr/lib/httpd/

uninstall:
	rm -rf /usr/lib/httpd/libhttpdWwwidget.so

clean:
	cd Release && make clean
	rm -rf kohaerenzstiftung.h
	rm -rf httpd.h
	rm -rf binary
