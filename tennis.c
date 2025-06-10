#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MAX_SCORE 2

typedef struct {
    int x, y;
    int w, h;
    int speed;
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

// Объявление глобальных переменных
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

Paddle player, bot;
Ball ball;
GameState gameState = STATE_MENU;

int playerScore = 0;
int botScore = 0;

// Настройка сложности бота (1 = легко, 2 = средне, 3 = сложно)
const char* difficultyLabels[] = {"Easy", "Medium", "Hard"};
int botDifficulty = 2;  // по умолчанию "Средне"
int selectedDifficulty = 1; // по умолчанию "Средне"

const char* winnerText = NULL;

// AI параметры
int botSpeed = 6;
int reactionDelay = 0;
int frameCounter = 0;

// Объявление функций
int init();
void cleanUp();
void handleInput(SDL_Event* e, int* running);
void update();
void render();
void renderMenu();
void renderGameOver();
void resetBall();
void setupBotDifficulty(int level);
void renderScore();

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

        if (gameState == STATE_GAME) {
            update();
        }
        render();
        SDL_Delay(16); // ~60 FPS
    }

    cleanUp();
    return 0;
}

// Функция инициализации
int init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL Init Error: %s\n", SDL_GetError());
        return 0;
    }

    if (TTF_Init() != 0) {
        printf("TTF Init Error: %s\n", TTF_GetError());
        return 0;
    }

    window = SDL_CreateWindow("SDL Tennis", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return 0;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return 0;

    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 32);
    if (!font) {
        printf("Font load error: %s\n", TTF_GetError());
        return 0;
    }

    player.x = 50;
    player.y = SCREEN_HEIGHT / 2 - 50;
    player.w = 20;
    player.h = 100;
    player.speed = 8;

    bot.x = SCREEN_WIDTH - 70;
    bot.y = SCREEN_HEIGHT / 2 - 50;
    bot.w = 20;
    bot.h = 100;
    bot.speed = 6;

    return 1;
}

// Обработка ввода
void handleInput(SDL_Event* e, int* running) {
    if (e->type == SDL_QUIT) {
        *running = 0;
    }

    if (gameState == STATE_MENU) {
        if (e->type == SDL_KEYDOWN) {
            if (e->key.keysym.sym == SDLK_UP && selectedDifficulty > 0)
                selectedDifficulty--;
            if (e->key.keysym.sym == SDLK_DOWN && selectedDifficulty < 2)
                selectedDifficulty++;
            if (e->key.keysym.sym == SDLK_RETURN) {
                gameState = STATE_GAME;
                setupBotDifficulty(selectedDifficulty + 1); // переводим в 1..3
                resetBall();
            }
            if (e->key.keysym.sym == SDLK_ESCAPE) {
		*running = 0;
	    }
        }
    } else if (gameState == STATE_GAME) {
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_W] && player.y > 0) {
            player.y -= player.speed;
        }
        if (keys[SDL_SCANCODE_S] && player.y + player.h < SCREEN_HEIGHT) {
            player.y += player.speed;
        }
    } else if (gameState == STATE_GAMEOVER) {
    	if (e->type == SDL_KEYDOWN) {
	    if (e->key.keysym.sym == SDLK_RETURN) {
		// Сброс состояния и счёта
		playerScore = 0;
		botScore = 0;
		selectedDifficulty = 1;
		gameState = STATE_MENU;
	    } else if (e->key.keysym.sym == SDLK_ESCAPE) {
		*running = 0;
	    }
	}
    }
}

// Обновление логики игры
void update() {
    // Бот
    frameCounter++;
    if (frameCounter >= reactionDelay) {
        if (bot.y + bot.h / 2 < ball.y) bot.y += botSpeed;
        else if (bot.y + bot.h / 2 > ball.y) bot.y -= botSpeed;
        frameCounter = 0;
    }

    // Мяч
    ball.x += ball.vx;
    ball.y += ball.vy;

    if (ball.y <= 0 || ball.y + ball.size >= SCREEN_HEIGHT) ball.vy = -ball.vy;

    SDL_Rect ballRect = {
        (int)ball.x,
        (int)ball.y,
        ball.size,
        ball.size
    };
    SDL_Rect playerRect = {player.x, player.y, player.w, player.h};
    SDL_Rect botRect = {bot.x, bot.y, bot.w, bot.h};

    if (SDL_HasIntersection(&ballRect, &playerRect)) ball.vx = -ball.vx;
    if (SDL_HasIntersection(&ballRect, &botRect)) ball.vx = -ball.vx;

    if (ball.x < 0) {
        botScore++;
        resetBall();
    } else if (ball.x > SCREEN_WIDTH) {
        playerScore++;
        resetBall();
    }
    
    if (playerScore >= MAX_SCORE || botScore >= MAX_SCORE) {
        if (playerScore >= MAX_SCORE) {
	    winnerText = "You win!";
	} else if (botScore >= MAX_SCORE) {
	    winnerText = "You lose!";
	}
        gameState = STATE_GAMEOVER;
    }
}

// Отрисовка всех объектов
void render() {
    if (gameState == STATE_MENU) {
        renderMenu();
    } else if (gameState == STATE_GAME) {
        // printf("Ball: (%.2f, %.2f)\n", ball.x, ball.y); // дебаг мяча
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_Rect playerRect = {player.x, player.y, player.w, player.h};
        SDL_Rect botRect = {bot.x, bot.y, bot.w, bot.h};
        SDL_Rect ballRect = {
            (int)ball.x,
            (int)ball.y,
            ball.size,
            ball.size
        };
        // printf("Render ball at (%.1f, %.1f), size: %d\n", ball.x, ball.y, ball.size); // дебаг мяча

        SDL_RenderFillRect(renderer, &playerRect);
        SDL_RenderFillRect(renderer, &botRect);
        SDL_RenderFillRect(renderer, &ballRect);

        renderScore();
        SDL_RenderPresent(renderer);
    } else if (gameState == STATE_GAMEOVER) {
        renderGameOver();
    }
}


// Отрисовка счёта
void renderScore() {
    char scoreText[20];
    sprintf(scoreText, "%d : %d", playerScore, botScore);

    SDL_Color white = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText, white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect dst = {SCREEN_WIDTH / 2 - w / 2, 20, w, h};

    SDL_RenderCopy(renderer, texture, NULL, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Отрисовка меню
void renderMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color white = {255, 255, 255};
    SDL_Color yellow = {255, 255, 0};

    for (int i = 0; i < 3; i++) {
        SDL_Color color = (i == selectedDifficulty) ? yellow : white;

        SDL_Surface* surface = TTF_RenderText_Solid(font, difficultyLabels[i], color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);
        SDL_Rect dst = {
            SCREEN_WIDTH / 2 - w / 2,
            200 + i * 60,
            w,
            h
        };

        SDL_RenderCopy(renderer, texture, NULL, &dst);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    // Заголовок
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "Select difficulty:", white);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    int tw, th;
    SDL_QueryTexture(titleTexture, NULL, NULL, &tw, &th);
    SDL_Rect titleRect = {SCREEN_WIDTH / 2 - tw / 2, 100, tw, th};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    SDL_RenderPresent(renderer);
}

// Отрисовка геймовера
void renderGameOver() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color white = {255, 255, 255};

    // Победитель
    SDL_Surface* surface = TTF_RenderText_Solid(font, winnerText, white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect rect = {SCREEN_WIDTH / 2 - w / 2, 200, w, h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // Подсказка
    SDL_Surface* hintSurface = TTF_RenderText_Solid(font, "Enter -- again | Esc -- exit", white);
    SDL_Texture* hintTexture = SDL_CreateTextureFromSurface(renderer, hintSurface);
    SDL_QueryTexture(hintTexture, NULL, NULL, &w, &h);
    SDL_Rect hintRect = {SCREEN_WIDTH / 2 - w / 2, 300, w, h};
    SDL_RenderCopy(renderer, hintTexture, NULL, &hintRect);
    SDL_FreeSurface(hintSurface);
    SDL_DestroyTexture(hintTexture);

    SDL_RenderPresent(renderer);
}

// Сброс позиции мяча в центр
void resetBall() {
    ball.x = SCREEN_WIDTH / 2.0f;
    ball.y = SCREEN_HEIGHT / 2.0f;
    ball.vx = (rand() % 2 == 0) ? 4.5f : -4.5f;
    ball.vy = (rand() % 2 == 0) ? 3.5f : -3.5f;
    ball.size = 10;
    frameCounter = 0;
}

// Настройка параметров бота по уровню сложности
void setupBotDifficulty(int level) {
    if (level == 1) {
        reactionDelay = 10;
        botSpeed = 4;
    } else if (level == 2) {
        reactionDelay = 5;
        botSpeed = 6;
    } else {
        reactionDelay = 0;
        botSpeed = 8;
    }
}

// Очистка ресурсов
void cleanUp() {
    if (font) {
        TTF_CloseFont(font);
        font = NULL;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    TTF_Quit();
    SDL_Quit();
}

