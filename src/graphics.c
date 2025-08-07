#include "graphics.h"
#include "game.h"
#include "input.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

static void renderScore();
static void renderMenu();
static void renderGameOver();

void render() {
    GameState* state = getGameState();

    if (*state == STATE_MENU) {
        renderMenu();
    } else if (*state == STATE_GAME) {
        SDL_SetRenderDrawColor(getRenderer(), 0, 0, 0, 255);
        SDL_RenderClear(getRenderer());

        SDL_SetRenderDrawColor(getRenderer(), 255, 255, 255, 255);

        Paddle* player = getPlayer();
        Paddle* bot = getBot();
        Ball* ball = getBall();

        SDL_Rect playerRect = {player->x, player->y, player->w, player->h};
        SDL_Rect botRect = {bot->x, bot->y, bot->w, bot->h};
        SDL_Rect ballRect = {(int)ball->x, (int)ball->y, ball->size, ball->size};

        SDL_RenderFillRect(getRenderer(), &playerRect);
        SDL_RenderFillRect(getRenderer(), &botRect);
        SDL_RenderFillRect(getRenderer(), &ballRect);

        renderScore();
        SDL_RenderPresent(getRenderer());
    } else if (*state == STATE_GAMEOVER) {
        renderGameOver();
    }
}

static void renderScore() {
    char scoreText[20];
    sprintf(scoreText, "%d : %d", playerScore, botScore);

    SDL_Color white = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(getFont(), scoreText, white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer(), surface);

    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect dst = {getScreenWidth() / 2 - w / 2, 20, w, h};

    SDL_RenderCopy(getRenderer(), texture, NULL, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

static void renderMenu() {
    SDL_SetRenderDrawColor(getRenderer(), 0, 0, 0, 255);
    SDL_RenderClear(getRenderer());

    SDL_Color white = {255, 255, 255};
    SDL_Color yellow = {255, 255, 0};

    for (int i = 0; i < 3; i++) {
        SDL_Color color = (i == selectedDifficulty) ? yellow : white;
        SDL_Surface* surface = TTF_RenderText_Solid(getFont(), difficultyLabels[i], color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer(), surface);

        int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);
        SDL_Rect dst = {getScreenWidth() / 2 - w / 2, 200 + i * 60, w, h};

        SDL_RenderCopy(getRenderer(), texture, NULL, &dst);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    SDL_Surface* titleSurface = TTF_RenderText_Solid(getFont(), "Select difficulty:", white);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(getRenderer(), titleSurface);
    int tw, th;
    SDL_QueryTexture(titleTexture, NULL, NULL, &tw, &th);
    SDL_Rect titleRect = {getScreenWidth() / 2 - tw / 2, 100, tw, th};
    SDL_RenderCopy(getRenderer(), titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    SDL_RenderPresent(getRenderer());
}

static void renderGameOver() {
    SDL_SetRenderDrawColor(getRenderer(), 0, 0, 0, 255);
    SDL_RenderClear(getRenderer());

    SDL_Color white = {255, 255, 255};

    SDL_Surface* surface = TTF_RenderText_Solid(getFont(), *getWinnerText(), white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer(), surface);
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect rect = {getScreenWidth() / 2 - w / 2, 200, w, h};
    SDL_RenderCopy(getRenderer(), texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_Surface* hintSurface = TTF_RenderText_Solid(getFont(), "Enter -- again | Esc -- exit", white);
    SDL_Texture* hintTexture = SDL_CreateTextureFromSurface(getRenderer(), hintSurface);
    SDL_QueryTexture(hintTexture, NULL, NULL, &w, &h);
    SDL_Rect hintRect = {getScreenWidth() / 2 - w / 2, 300, w, h};
    SDL_RenderCopy(getRenderer(), hintTexture, NULL, &hintRect);
    SDL_FreeSurface(hintSurface);
    SDL_DestroyTexture(hintTexture);

    SDL_RenderPresent(getRenderer());
}
