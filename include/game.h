#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    int x, y, w, h, speed;
} Paddle;

typedef struct {
    float x, y;
    float vx, vy;
    int size;
} Ball;

typedef enum {
    STATE_MENU,
    STATE_GAME,
    STATE_GAMEOVER
} GameState;

int init();
void update();
void resetBall();
void cleanUp();

extern int playerScore;
extern int botScore;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

SDL_Renderer* getRenderer();
TTF_Font* getFont();
GameState* getGameState();
Paddle* getPlayer();
Paddle* getBot();
Ball* getBall();
const char** getWinnerText();
int* getFrameCounter();
int getScreenWidth();
int getScreenHeight();

#endif
