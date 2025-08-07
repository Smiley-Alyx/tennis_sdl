#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

void handleInput(SDL_Event* e, int* running);

extern int selectedDifficulty;
extern const char* difficultyLabels[3];

#endif
