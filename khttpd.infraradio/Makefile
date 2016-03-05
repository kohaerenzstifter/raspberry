KOHAERENZSTIFTUNG := ../kohaerenzstiftung/
KHTTPD := ../khttpd/

all: binary/libhttpdInfraradio.so

binary/libhttpdInfraradio.so:
	cp $(KOHAERENZSTIFTUNG)/kohaerenzstiftung.h .
	cp $(KHTTPD)/src/httpd.h .
	cd Release && make
	-mkdir -p binary
	cp Release/libhttpdInfraradio.so binary/libhttpdInfraradio.so

install: all
	install -m 644 binary/libhttpdInfraradio.so /usr/lib/httpd/

uninstall:
	rm -rf /usr/lib/httpd/libhttpdInfraradio.so

clean:
	cd Release && make clean
	rm -rf kohaerenzstiftung.h
	rm -rf httpd.h
	rm -rf binary
