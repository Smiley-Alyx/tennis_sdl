#include "game.h"
#include "bot.h"
#include "sound.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>

#define MAX_SCORE 10

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

Paddle player, bot;
Ball ball;
GameState gameState = STATE_MENU;

int playerScore = 0;
int botScore = 0;
int playerCount = 1;
const char* winnerText = NULL;

int frameCounter = 0;

// Геттеры для доступа из других модулей
SDL_Renderer* getRenderer() { return renderer; }
TTF_Font* getFont() { return font; }
GameState* getGameState() { return &gameState; }
Paddle* getPlayer() { return &player; }
Paddle* getBot() { return &bot; }
Ball* getBall() { return &ball; }
const char** getWinnerText() { return &winnerText; }
int* getFrameCounter() { return &frameCounter; }

int getScreenWidth() { return SCREEN_WIDTH; }
int getScreenHeight() { return SCREEN_HEIGHT; }

// Инициализация SDL и игровых объектов
int init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) return 0;
    if (TTF_Init() != 0) return 0;

    window = SDL_CreateWindow("SDL Tennis", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return 0;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return 0;

    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 32);
    if (!font) return 0;

    player.w = 20;
    player.h = 100;
    player.speed = 8;

    bot.w = 20;
    bot.h = 100;
    bot.speed = 6;
    resetPaddles();

    initSound();

    return 1;
}

// Обновление логики игры
void update() {
    Paddle* bot = getBot();
    Ball* ball = getBall();

    if (playerCount == 1) {
        frameCounter++;
        if (frameCounter >= getReactionDelay()) {
            if (bot->y + bot->h / 2 < ball->y) bot->y += getBotSpeed();
            else if (bot->y + bot->h / 2 > ball->y) bot->y -= getBotSpeed();

            if (bot->y < 0) {
                bot->y = 0;
            }
            if (bot->y + bot->h > SCREEN_HEIGHT) {
                bot->y = SCREEN_HEIGHT - bot->h;
            }

            frameCounter = 0;
        }
    }

    ball->x += ball->vx;
    ball->y += ball->vy;

    if (ball->y <= 0 && ball->vy < 0) {
        ball->vy = -ball->vy;
        ball->y = 0;
        playSound(SOUND_WALL);
    } else if (ball->y + ball->size >= SCREEN_HEIGHT && ball->vy > 0) {
        ball->vy = -ball->vy;
        ball->y = SCREEN_HEIGHT - ball->size;
        playSound(SOUND_WALL);
    }

    SDL_Rect ballRect = {(int)ball->x, (int)ball->y, ball->size, ball->size};
    SDL_Rect playerRect = {player.x, player.y, player.w, player.h};
    SDL_Rect botRect = {bot->x, bot->y, bot->w, bot->h};

    if (SDL_HasIntersection(&ballRect, &playerRect) && ball->vx < 0) {
        ball->vx = -ball->vx;
        ball->x = player.x + player.w;
        playSound(SOUND_PADDLE);
    }
    if (SDL_HasIntersection(&ballRect, &botRect) && ball->vx > 0) {
        ball->vx = -ball->vx;
        ball->x = bot->x - ball->size;
        playSound(SOUND_PADDLE);
    }

    if (ball->x < 0) {
        botScore++;
        playSound(SOUND_SCORE);
        resetBall();
    } else if (ball->x > SCREEN_WIDTH) {
        playerScore++;
        playSound(SOUND_SCORE);
        resetBall();
    }

    if (playerScore >= MAX_SCORE || botScore >= MAX_SCORE) {
        if (playerCount == 1) {
            winnerText = playerScore >= MAX_SCORE ? "You win!" : "You lose!";
        } else {
            winnerText = playerScore >= MAX_SCORE ? "Player 1 wins!" : "Player 2 wins!";
        }
        playSound(SOUND_GAME_OVER);
        *getGameState() = STATE_GAMEOVER;
    }
}

// Сброс позиций ракеток
void resetPaddles() {
    player.x = 50;
    player.y = SCREEN_HEIGHT / 2 - player.h / 2;

    bot.x = SCREEN_WIDTH - 70;
    bot.y = SCREEN_HEIGHT / 2 - bot.h / 2;
}

// Сброс позиции мяча
void resetBall() {
    Ball* ball = getBall();
    ball->x = SCREEN_WIDTH / 2.0f;
    ball->y = SCREEN_HEIGHT / 2.0f;
    ball->vx = (rand() % 2 == 0) ? 4.5f : -4.5f;
    ball->vy = (rand() % 2 == 0) ? 3.5f : -3.5f;
    ball->size = 10;
    frameCounter = 0;
}

// Очистка ресурсов SDL
void cleanUp() {
    cleanUpSound();
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}
