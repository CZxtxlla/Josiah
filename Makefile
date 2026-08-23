CC = gcc
CFLAGS = -Wall -g

SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)

all: Josiah

Josiah: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f src/*.o Josiah