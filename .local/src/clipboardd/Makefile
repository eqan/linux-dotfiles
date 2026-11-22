CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -std=c11
PREFIX ?= $(HOME)/.local

all: clipboardd clipboard-show

clipboardd: clipboardd.c
	$(CC) $(CFLAGS) -o $@ $< -lX11 -lXfixes -lsqlite3

clipboard-show: clipboard-show.c
	$(CC) $(CFLAGS) -o $@ $<

install: clipboardd clipboard-show
	install -Dm755 clipboardd $(PREFIX)/bin/clipboardd
	install -Dm755 clipboard-show $(PREFIX)/bin/clipboard-show

clean:
	rm -f clipboardd clipboard-show
