#include "input.h"
#include "game.h"

int selectedDifficulty = 1;
const char* difficultyLabels[3] = {"Easy", "Medium", "Hard"};

void handleInput(SDL_Event* e, int* running) {
    GameState* gameState = getGameState();
    Paddle* player = getPlayer();

    if (e->type == SDL_QUIT) {
        *running = 0;
        return;
    }

    if (*gameState == STATE_MENU) {
        if (e->type == SDL_KEYDOWN) {
            if (e->key.keysym.sym == SDLK_UP && selectedDifficulty > 0)
                selectedDifficulty--;
            if (e->key.keysym.sym == SDLK_DOWN && selectedDifficulty < 2)
                selectedDifficulty++;
            if (e->key.keysym.sym == SDLK_RETURN) {
                *gameState = STATE_GAME;
                setupBotDifficulty(selectedDifficulty + 1);
                resetBall();
            }
            if (e->key.keysym.sym == SDLK_ESCAPE) {
                *running = 0;
            }
        }
    } else if (*gameState == STATE_GAME) {
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_W] && player->y > 0) {
            player->y -= player->speed;
        }
        if (keys[SDL_SCANCODE_S] && player->y + player->h < SCREEN_HEIGHT) {
            player->y += player->speed;
        }
    } else if (*gameState == STATE_GAMEOVER) {
        if (e->type == SDL_KEYDOWN) {
            if (e->key.keysym.sym == SDLK_RETURN) {
                playerScore = 0;
                botScore = 0;
                selectedDifficulty = 1;
                *gameState = STATE_MENU;
            } else if (e->key.keysym.sym == SDLK_ESCAPE) {
                *running = 0;
            }
        }
    }
}
