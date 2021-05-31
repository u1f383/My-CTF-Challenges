CC := gcc
CFLAGS := -g -Wall

.PHONY: all

all: sleeper

sleeper: sleeper.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm sleeper