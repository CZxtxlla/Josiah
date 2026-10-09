CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -std=gnu11

SOURCES = $(wildcard src/*.c src/syzygy/*.c)
OBJECTS = $(SOURCES:.c=.o)

all: release

release: CFLAGS += -O3 -march=native -flto -DNDEBUG
release: LDFLAGS += -flto
release: Josiah

src/syzygy/tbprobe.o: CFLAGS += -Wno-sign-compare -Wno-unused-parameter

debug: CFLAGS += -g -O0
debug: Josiah

Josiah: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all release debug clean

clean:
	rm -f $(OBJECTS) Josiah