TARGET := mooshak-cli

SRC := $(wildcard *.c)
OBJ := $(patsubst %.c,%.o,$(SRC))

CC := gcc
CFLAGS := -Wall -pedantic -g -O0 -I../libmooshak -D_DEBUG_
LDFLAGS := -L../libmooshak -l:libmooshak.a -lbsd -lcurl

all: $(TARGET)

$(TARGET): $(OBJ) ../libmooshak/libmooshak.a
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJ)
