CC = gcc
PKG_CONFIG = pkg-config

ifeq ($(OS),Windows_NT)
EXE = .exe
SDL_FALLBACK_LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
else
SDL_CONFIG_LIBS = $(shell sdl2-config --libs 2>/dev/null)
SDL_FALLBACK_LIBS = $(if $(SDL_CONFIG_LIBS),$(SDL_CONFIG_LIBS),-lSDL2) -lSDL2_ttf
endif

SDL_CFLAGS = $(shell $(PKG_CONFIG) --cflags sdl2 SDL2_ttf 2>/dev/null)
SDL_LIBS = $(shell $(PKG_CONFIG) --libs sdl2 SDL2_ttf 2>/dev/null)

ifeq ($(strip $(SDL_CFLAGS)$(SDL_LIBS)),)
SDL_CFLAGS = $(shell sdl2-config --cflags 2>/dev/null)
SDL_LIBS = $(SDL_FALLBACK_LIBS)
endif

CFLAGS = -Wall -Wextra -Iinclude -std=c11 $(SDL_CFLAGS)
LDFLAGS = $(SDL_LIBS)

SRC = src/main.c src/game.c src/input.c src/graphics.c src/bot.c src/sound.c src/config.c
OBJ = $(SRC:.c=.o)
BIN = tennis$(EXE)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(BIN)
