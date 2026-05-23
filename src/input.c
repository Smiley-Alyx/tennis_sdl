#include "input.h"
#include "game.h"
#include "bot.h"
#include "sound.h"

int selectedDifficulty = 1;
int selectedMenuItem = 0;
const char* difficultyLabels[3] = {"Easy", "Medium", "Hard"};
const char* playerCountLabels[2] = {"1 Player", "2 Players"};

int getMenuItemCount() {
    return playerCount == 1 ? 3 : 2;
}

int getStartMenuItem() {
    return getMenuItemCount() - 1;
}

void handleInput(SDL_Event* e, int* running) {
    GameState* gameState = getGameState();

    if (e->type == SDL_QUIT) {
        *running = 0;
        return;
    }

    if (*gameState == STATE_MENU) {
        if (e->type == SDL_KEYDOWN) {
            int menuItemCount = getMenuItemCount();

            if (e->key.keysym.sym == SDLK_UP && selectedMenuItem > 0) {
                selectedMenuItem--;
                playSound(SOUND_MENU);
            }
            if (e->key.keysym.sym == SDLK_DOWN && selectedMenuItem < menuItemCount - 1) {
                selectedMenuItem++;
                playSound(SOUND_MENU);
            }
            if (e->key.keysym.sym == SDLK_LEFT || e->key.keysym.sym == SDLK_RIGHT) {
                if (selectedMenuItem == 0) {
                    playerCount = playerCount == 1 ? 2 : 1;
                    if (selectedMenuItem >= getMenuItemCount()) {
                        selectedMenuItem = getStartMenuItem();
                    }
                    playSound(SOUND_MENU);
                } else if (playerCount == 1 && selectedMenuItem == 1) {
                    if (e->key.keysym.sym == SDLK_LEFT && selectedDifficulty > 0) {
                        selectedDifficulty--;
                        playSound(SOUND_MENU);
                    }
                    if (e->key.keysym.sym == SDLK_RIGHT && selectedDifficulty < 2) {
                        selectedDifficulty++;
                        playSound(SOUND_MENU);
                    }
                }
            }
            if (e->key.keysym.sym == SDLK_RETURN && selectedMenuItem == getStartMenuItem()) {
                playSound(SOUND_MENU);
                *gameState = STATE_GAME;
                playerScore = 0;
                botScore = 0;
                resetPaddles();
                if (playerCount == 1) {
                    setupBotDifficulty(selectedDifficulty + 1);
                }
                resetBall();
            }
            if (e->key.keysym.sym == SDLK_ESCAPE) {
                *running = 0;
            }
        }
    } else if (*gameState == STATE_GAMEOVER) {
        if (e->type == SDL_KEYDOWN) {
            if (e->key.keysym.sym == SDLK_RETURN) {
                playerScore = 0;
                botScore = 0;
                selectedDifficulty = 1;
                selectedMenuItem = 0;
                playerCount = 1;
                *gameState = STATE_MENU;
            } else if (e->key.keysym.sym == SDLK_ESCAPE) {
                *running = 0;
            }
        }
    }
}

void handlePlayerInput() {
    Paddle* player = getPlayer();
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_W]) {
        player->y -= player->speed;
    }
    if (keys[SDL_SCANCODE_S]) {
        player->y += player->speed;
    }

    if (player->y < getPlayfieldTop()) {
        player->y = getPlayfieldTop();
    }
    if (player->y + player->h > getPlayfieldBottom()) {
        player->y = getPlayfieldBottom() - player->h;
    }

    if (playerCount == 2) {
        Paddle* secondPlayer = getBot();

        if (keys[SDL_SCANCODE_UP]) {
            secondPlayer->y -= secondPlayer->speed;
        }
        if (keys[SDL_SCANCODE_DOWN]) {
            secondPlayer->y += secondPlayer->speed;
        }

        if (secondPlayer->y < getPlayfieldTop()) {
            secondPlayer->y = getPlayfieldTop();
        }
        if (secondPlayer->y + secondPlayer->h > getPlayfieldBottom()) {
            secondPlayer->y = getPlayfieldBottom() - secondPlayer->h;
        }
    }
}
