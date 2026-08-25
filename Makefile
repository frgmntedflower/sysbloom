CC      ?= cc
CFLAGS  ?= -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -O2
PREFIX  ?= /usr/local
BINDIR  := $(PREFIX)/bin

TARGET  := sysbloom
SRC     := sysbloom.c

.PHONY: all install uninstall clean

all: config.h $(TARGET)

config.h:
	cp config.def.h config.h

$(TARGET): $(SRC) sbl.h config.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)

clean:
	rm -f $(TARGET)
