#include "input.h"
#include "game.h"
#include "bot.h"
#include "config.h"
#include "sound.h"

int selectedDifficulty = 1;
int selectedMenuItem = 0;
const char* difficultyLabels[3] = {"Easy", "Medium", "Hard"};
const char* playerCountLabels[2] = {"1 Player", "2 Players"};
static const int targetScoreValues[3] = {5, 10, 15};

typedef enum {
    MENU_PLAYERS,
    MENU_DIFFICULTY,
    MENU_MUSIC,
    MENU_SOUND,
    MENU_SCORE,
    MENU_START
} MenuItem;

static MenuItem getMenuItem(int index) {
    if (index == 0) return MENU_PLAYERS;
    if (playerCount == 1 && index == 1) return MENU_DIFFICULTY;
    if (index == getMenuItemCount() - 4) return MENU_MUSIC;
    if (index == getMenuItemCount() - 3) return MENU_SOUND;
    if (index == getMenuItemCount() - 2) return MENU_SCORE;
    return MENU_START;
}

static int getTargetScoreIndex() {
    for (int i = 0; i < 3; i++) {
        if (getConfig()->targetScore == targetScoreValues[i]) return i;
    }
    return 1;
}

static void startGame(GameState* gameState) {
    Config* config = getConfig();

    config->playerCount = playerCount;
    config->difficulty = selectedDifficulty;
    saveConfig();

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

int getMenuItemCount() {
    return playerCount == 1 ? 6 : 5;
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
                MenuItem item = getMenuItem(selectedMenuItem);

                if (item == MENU_PLAYERS) {
                    playerCount = playerCount == 1 ? 2 : 1;
                    getConfig()->playerCount = playerCount;
                    if (selectedMenuItem >= getMenuItemCount()) {
                        selectedMenuItem = getStartMenuItem();
                    }
                    saveConfig();
                    playSound(SOUND_MENU);
                } else if (item == MENU_DIFFICULTY) {
                    if (e->key.keysym.sym == SDLK_LEFT && selectedDifficulty > 0) {
                        selectedDifficulty--;
                        getConfig()->difficulty = selectedDifficulty;
                        saveConfig();
                        playSound(SOUND_MENU);
                    }
                    if (e->key.keysym.sym == SDLK_RIGHT && selectedDifficulty < 2) {
                        selectedDifficulty++;
                        getConfig()->difficulty = selectedDifficulty;
                        saveConfig();
                        playSound(SOUND_MENU);
                    }
                } else if (item == MENU_MUSIC) {
                    getConfig()->musicEnabled = !getConfig()->musicEnabled;
                    saveConfig();
                    playSound(SOUND_MENU);
                } else if (item == MENU_SOUND) {
                    getConfig()->soundEnabled = !getConfig()->soundEnabled;
                    saveConfig();
                    playSound(SOUND_MENU);
                } else if (item == MENU_SCORE) {
                    int direction = e->key.keysym.sym == SDLK_RIGHT ? 1 : -1;
                    int scoreIndex = getTargetScoreIndex() + direction;
                    if (scoreIndex < 0) scoreIndex = 2;
                    if (scoreIndex > 2) scoreIndex = 0;
                    getConfig()->targetScore = targetScoreValues[scoreIndex];
                    saveConfig();
                    playSound(SOUND_MENU);
                }
            }
            if (e->key.keysym.sym == SDLK_RETURN) {
                MenuItem item = getMenuItem(selectedMenuItem);

                if (item == MENU_START) {
                    startGame(gameState);
                } else if (item == MENU_MUSIC) {
                    getConfig()->musicEnabled = !getConfig()->musicEnabled;
                    saveConfig();
                    playSound(SOUND_MENU);
                } else if (item == MENU_SOUND) {
                    getConfig()->soundEnabled = !getConfig()->soundEnabled;
                    saveConfig();
                    playSound(SOUND_MENU);
                } else if (item == MENU_PLAYERS) {
                    playerCount = playerCount == 1 ? 2 : 1;
                    getConfig()->playerCount = playerCount;
                    if (selectedMenuItem >= getMenuItemCount()) {
                        selectedMenuItem = getStartMenuItem();
                    }
                    saveConfig();
                    playSound(SOUND_MENU);
                } else if (item == MENU_SCORE) {
                    int scoreIndex = getTargetScoreIndex() + 1;
                    if (scoreIndex > 2) scoreIndex = 0;
                    getConfig()->targetScore = targetScoreValues[scoreIndex];
                    saveConfig();
                    playSound(SOUND_MENU);
                } else if (item == MENU_DIFFICULTY) {
                    selectedDifficulty++;
                    if (selectedDifficulty > 2) selectedDifficulty = 0;
                    getConfig()->difficulty = selectedDifficulty;
                    saveConfig();
                    playSound(SOUND_MENU);
                }
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
                *gameState = STATE_MENU;
            } else if (e->key.keysym.sym == SDLK_r) {
                startGame(gameState);
            } else if (e->key.keysym.sym == SDLK_ESCAPE) {
                *running = 0;
            }
        }
    }
}

void handlePlayerInput() {
    Paddle* player = getPlayer();
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    Config* config = getConfig();

    if (keys[config->playerUp]) {
        player->y -= player->speed;
    }
    if (keys[config->playerDown]) {
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

        if (keys[config->secondPlayerUp]) {
            secondPlayer->y -= secondPlayer->speed;
        }
        if (keys[config->secondPlayerDown]) {
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
