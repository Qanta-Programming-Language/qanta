CC = gcc
CFLAGS = -Wall -Iinclude

SRC = src/main.c src/lexer.c src/parser.c src/compiler.c
OBJ = $(SRC:.c=.o)
BIN = qanta

all:
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

clean:
	rm -f $(BIN) *.o