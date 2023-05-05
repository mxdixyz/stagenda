# See LICENSE file for copyright and license details.
CC = gcc
PREFIX = /usr/local


all:
	$(CC) -g -o stagenda stagenda.c

install: all
	mkdir -p "$(PREFIX)/bin"
	cp -f stagenda "$(PREFIX)/bin"
	chmod 755 "$(PREFIX)/bin/stagenda"

uninstall:
	rm -f "$(PREFIX)/bin/stagenda"