#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

void handleInput(SDL_Event* e, int* running);
void handlePlayerInput();
int getMenuItemCount();
int getStartMenuItem();

extern int selectedDifficulty;
extern int selectedMenuItem;
extern const char* difficultyLabels[3];
extern const char* playerCountLabels[2];

#endif
