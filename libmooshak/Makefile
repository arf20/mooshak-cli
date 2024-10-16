TARGET := libmooshak.a
OBJ := libmooshak.o
CC := gcc
CFLAGS := -Wall -pedantic -g -O0 -D_DEBUG_
LDFLAGS := -lbsd -lcurl

SRC := $(wildcard *.c)

all: $(TARGET)

$(TARGET): $(OBJ)
	ar rcs $(TARGET) $(OBJ)

$(OBJ): $(SRC)
	$(CC) -c $(CFLAGS) *.c $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET) *.o
