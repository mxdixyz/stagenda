# See LICENSE file for copyright and license details.
CC = gcc
PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

all:
	$(CC) -g -c stagenda.c
	$(CC) -g -o stagenda stagenda.o

install: all
	mkdir -p "$(PREFIX)/bin"
	cp -f stagenda "$(PREFIX)/bin"
	chmod 755 "$(PREFIX)/bin/stagenda"

	mkdir -p "$(MANPREFIX)/man1"
	cp -f stagenda.1 "$(MANPREFIX)/man1"
	chmod 644 "$(MANPREFIX)/man1/stagenda.1"

uninstall:
	rm -f "$(PREFIX)/bin/stagenda" "$(MANPREFIX)/man1/stagenda.1"

clean:
	rm -f stagenda stagenda.o