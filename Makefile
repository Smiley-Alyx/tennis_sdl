CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -std=c11 `sdl2-config --cflags` -lSDL2_ttf
LDFLAGS = `sdl2-config --libs` -lSDL2_ttf

SRC = src/main.c src/game.c src/input.c src/graphics.c src/bot.c
OBJ = $(SRC:.c=.o)
BIN = tennis

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(BIN)
