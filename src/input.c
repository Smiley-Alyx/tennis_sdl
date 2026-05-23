#include "input.h"
#include "game.h"
#include "bot.h"
#include "config.h"
#include "sound.h"

int selectedDifficulty = 1;
int selectedMenuItem = 0;
int selectedPauseItem = 0;
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

typedef enum {
    PAUSE_RESUME,
    PAUSE_MUSIC,
    PAUSE_SOUND,
    PAUSE_MENU
} PauseItem;

static MenuItem getMenuItem(int index) {
    if (index == 0) return MENU_PLAYERS;
    if (playerCount == 1 && index == 1) return MENU_DIFFICULTY;
    if (index == getMenuItemCount() - 4) return MENU_MUSIC;
    if (index == getMenuItemCount() - 3) return MENU_SOUND;
    if (index == getMenuItemCount() - 2) return MENU_SCORE;
    return MENU_START;
}

static PauseItem getPauseItem(int index) {
    if (index == 0) return PAUSE_RESUME;
    if (index == 1) return PAUSE_MUSIC;
    if (index == 2) return PAUSE_SOUND;
    return PAUSE_MENU;
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

static void returnToMenu(GameState* gameState) {
    playerScore = 0;
    botScore = 0;
    selectedMenuItem = getStartMenuItem();
    *gameState = STATE_MENU;
}

static void saveMenuChange() {
    saveConfig();
    playSound(SOUND_MENU);
}

static void togglePlayerCount() {
    playerCount = playerCount == 1 ? 2 : 1;
    getConfig()->playerCount = playerCount;
    if (selectedMenuItem >= getMenuItemCount()) {
        selectedMenuItem = getStartMenuItem();
    }
    saveMenuChange();
}

static void changeDifficulty(int direction, int wrap) {
    int difficulty = selectedDifficulty + direction;

    if (wrap) {
        if (difficulty > 2) difficulty = 0;
        if (difficulty < 0) difficulty = 2;
    } else if (difficulty < 0 || difficulty > 2) {
        return;
    }

    selectedDifficulty = difficulty;
    getConfig()->difficulty = selectedDifficulty;
    saveMenuChange();
}

static void toggleMusic() {
    getConfig()->musicEnabled = !getConfig()->musicEnabled;
    saveMenuChange();
}

static void toggleSound() {
    getConfig()->soundEnabled = !getConfig()->soundEnabled;
    saveMenuChange();
}

static void changeTargetScore(int direction) {
    int scoreIndex = getTargetScoreIndex() + direction;

    if (scoreIndex < 0) scoreIndex = 2;
    if (scoreIndex > 2) scoreIndex = 0;

    getConfig()->targetScore = targetScoreValues[scoreIndex];
    saveMenuChange();
}

int getMenuItemCount() {
    return playerCount == 1 ? 6 : 5;
}

int getStartMenuItem() {
    return getMenuItemCount() - 1;
}

int getPauseMenuItemCount() {
    return 4;
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
                    togglePlayerCount();
                } else if (item == MENU_DIFFICULTY) {
                    int direction = e->key.keysym.sym == SDLK_RIGHT ? 1 : -1;
                    changeDifficulty(direction, 0);
                } else if (item == MENU_MUSIC) {
                    toggleMusic();
                } else if (item == MENU_SOUND) {
                    toggleSound();
                } else if (item == MENU_SCORE) {
                    int direction = e->key.keysym.sym == SDLK_RIGHT ? 1 : -1;
                    changeTargetScore(direction);
                }
            }
            if (e->key.keysym.sym == SDLK_RETURN) {
                MenuItem item = getMenuItem(selectedMenuItem);

                if (item == MENU_START) {
                    startGame(gameState);
                } else if (item == MENU_MUSIC) {
                    toggleMusic();
                } else if (item == MENU_SOUND) {
                    toggleSound();
                } else if (item == MENU_PLAYERS) {
                    togglePlayerCount();
                } else if (item == MENU_SCORE) {
                    changeTargetScore(1);
                } else if (item == MENU_DIFFICULTY) {
                    changeDifficulty(1, 1);
                }
            }
            if (e->key.keysym.sym == SDLK_ESCAPE) {
                *running = 0;
            }
        }
    } else if (*gameState == STATE_GAME) {
        if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE) {
            selectedPauseItem = 0;
            *gameState = STATE_PAUSE;
            playSound(SOUND_MENU);
        }
    } else if (*gameState == STATE_PAUSE) {
        if (e->type == SDL_KEYDOWN) {
            int pauseMenuItemCount = getPauseMenuItemCount();

            if (e->key.keysym.sym == SDLK_UP && selectedPauseItem > 0) {
                selectedPauseItem--;
                playSound(SOUND_MENU);
            }
            if (e->key.keysym.sym == SDLK_DOWN && selectedPauseItem < pauseMenuItemCount - 1) {
                selectedPauseItem++;
                playSound(SOUND_MENU);
            }
            if (e->key.keysym.sym == SDLK_ESCAPE) {
                *gameState = STATE_GAME;
                playSound(SOUND_MENU);
            }
            if (e->key.keysym.sym == SDLK_LEFT || e->key.keysym.sym == SDLK_RIGHT
                    || e->key.keysym.sym == SDLK_RETURN) {
                PauseItem item = getPauseItem(selectedPauseItem);

                if (item == PAUSE_RESUME) {
                    *gameState = STATE_GAME;
                    playSound(SOUND_MENU);
                } else if (item == PAUSE_MUSIC) {
                    toggleMusic();
                } else if (item == PAUSE_SOUND) {
                    toggleSound();
                } else if (item == PAUSE_MENU) {
                    playSound(SOUND_MENU);
                    returnToMenu(gameState);
                }
            }
        }
    } else if (*gameState == STATE_GAMEOVER) {
        if (e->type == SDL_KEYDOWN) {
            if (e->key.keysym.sym == SDLK_RETURN) {
                returnToMenu(gameState);
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
