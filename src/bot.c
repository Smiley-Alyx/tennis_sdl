#include "bot.h"

static int botSpeed = 6;
static int reactionDelay = 0;

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

int getBotSpeed() {
    return botSpeed;
}

int getReactionDelay() {
    return reactionDelay;
}
