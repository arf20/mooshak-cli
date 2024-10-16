PROJECT := mooshak-cli
CC := gcc
CFLAGS := -Wall -pedantic -g -O0 -D_DEBUG_
LDFLAGS := -lbsd -lcurl

SRC := $(wildcard *.c)

all: $(PROJECT)

$(PROJECT): $(SRC)
	$(CC) $(CFLAGS) -o $(PROJECT) *.c $(LDFLAGS)

.PHONY: clean
clean:
	rm $(PROJECT)
