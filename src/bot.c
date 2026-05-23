#include "bot.h"

static int botSpeed = 6;
static int reactionDelay = 0;
static int aimOffset = 0;

void setupBotDifficulty(int level) {
    if (level == 0) {
        reactionDelay = 6;
        botSpeed = 5;
        aimOffset = 26;
    } else if (level == 1) {
        reactionDelay = 3;
        botSpeed = 6;
        aimOffset = 14;
    } else {
        reactionDelay = 2;
        botSpeed = 7;
        aimOffset = 6;
    }
}

int getBotSpeed() {
    return botSpeed;
}

int getReactionDelay() {
    return reactionDelay;
}

int getBotAimOffset() {
    return aimOffset;
}
