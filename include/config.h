#ifndef CONFIG_H
#define CONFIG_H

#include <SDL2/SDL.h>

typedef struct {
    int playerCount;
    int difficulty;
    int musicEnabled;
    int soundEnabled;
    int targetScore;
    SDL_Scancode playerUp;
    SDL_Scancode playerDown;
    SDL_Scancode secondPlayerUp;
    SDL_Scancode secondPlayerDown;
} Config;

void loadConfig();
void saveConfig();
Config* getConfig();

#endif
