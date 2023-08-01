CC=gcc
CFLAGS=-fdiagnostics-color=always -g -Wall -Wextra -Wshadow -Wpedantic
OBJ=./bt_test.o ./genbinarytree.o
BIN=./build

all: $(BIN)

test: $(BIN)
	./build

build: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) *.o