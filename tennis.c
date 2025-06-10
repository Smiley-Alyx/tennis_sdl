#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Настройка сложности бота (1 = легко, 2 = средне, 3 = сложно)
#define BOT_DIFFICULTY 2

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

// Объявление глобальных переменных
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

Paddle player, bot;
Ball ball;

int playerScore = 0;
int botScore = 0;

// Объявление функций
int init();
void cleanUp();
void handleInput(SDL_Event* e, int* running);
void update();
void render();
void resetBall();
void setupBotDifficulty();

int main() {
    if (!init()) {
        printf("Initialization failed.\n");
        return 1;
    }

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            handleInput(&event, &running);
        }

        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }

    cleanUp();
    return 0;
}

// Функция инициализации
int init() {
    // TODO: создать окно, рендерер, загрузить шрифт
    return 1;
}

// Обработка ввода
void handleInput(SDL_Event* e, int* running) {
    // TODO: реализовать управление игроком и выход
}

// Обновление логики игры
void update() {
    // TODO: реализовать движение, столкновения, очки
}

// Отрисовка всех объектов
void render() {
    // TODO: нарисовать фон, ракетки, мяч, счёт
}

// Сброс позиции мяча в центр
void resetBall() {
    // TODO: вернуть мяч в центр и задать начальную скорость
}

// Настройка параметров бота по уровню сложности
void setupBotDifficulty() {
    // TODO: установить скорость и задержку реакции в зависимости от BOT_DIFFICULTY
}

// Очистка ресурсов
void cleanUp() {
    // TODO: уничтожить текстуры, рендерер, окно, выгрузить TTF
}
