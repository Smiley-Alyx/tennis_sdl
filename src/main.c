#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"
#include "graphics.h"
#include "input.h"
#include "bot.h"

int main() {
    if (!init()) {
        printf("Initialization failed.\n");
        return 1;
    }

    setupBotDifficulty(selectedDifficulty + 1);
    resetBall();

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            handleInput(&event, &running);
        }

        if (*getGameState() == STATE_GAME) {
            update();
        }
        render();
        SDL_Delay(16); // ~60 FPS
    }

    cleanUp();
    return 0;
}
