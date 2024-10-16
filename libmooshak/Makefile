TARGET := libmooshak.a

SRC := $(wildcard *.c)
OBJ := $(patsubst %.c,%.o,$(SRC))

CC := gcc
CFLAGS := -Wall -pedantic -g -O0 -D_DEBUG_
LDFLAGS := 

all: $(TARGET)

$(TARGET): $(OBJ)
	ar rcs $(TARGET) $(OBJ)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJ)
