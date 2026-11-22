CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -std=c11
PREFIX ?= $(HOME)/.local

all: clipboardd

clipboardd: clipboardd.c
	$(CC) $(CFLAGS) -o $@ $< -lX11 -lXfixes -lsqlite3

install: clipboardd
	install -Dm755 clipboardd $(PREFIX)/bin/clipboardd

clean:
	rm -f clipboardd
