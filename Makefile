CC = cc
CFLAGS = -std=c99 -g -Wall -Wextra -pedantic -O2
PREFIX = /usr/local

SRC = sysbloom.c info.c
OBJ = $(SRC:.c=.o)

config.h:
	cp config.def.h config.h

sysbloom: config.h $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c config.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f sysbloom $(OBJ) config.h

install: sysbloom
	mkdir -p $(PREFIX)/bin
	cp sysbloom $(PREFIX)/bin/sysbloom

uninstall:
	rm -f $(PREFIX)/bin/sysbloom

.PHONY: clean install uninstall
