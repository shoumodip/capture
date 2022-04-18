CFLAGS=-Wall -Wextra -std=c11 -pedantic

.PHONY: all
all: main test

%: %.c
	$(CC) $(CFLAGS) -o $@ $<
