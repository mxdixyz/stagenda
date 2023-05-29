# See LICENSE file for copyright and license details.
CC = gcc
PREFIX = /usr/local

all:
	$(CC) -g -c stagenda.c
	$(CC) -g -o stagenda stagenda.o

install: all
	mkdir -p "$(PREFIX)/bin"
	cp -f stagenda "$(PREFIX)/bin"
	chmod 755 "$(PREFIX)/bin/stagenda"

uninstall:
	rm -f "$(PREFIX)/bin/stagenda"

clean:
	rm -f stagenda stagenda.o