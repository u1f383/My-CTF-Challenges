CC := gcc
CFLAGS := -g -Wall

.PHONY: all

all: noper

noper: noper.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm noper