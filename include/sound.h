#ifndef SOUND_H
#define SOUND_H

typedef enum {
    SOUND_MENU,
    SOUND_PADDLE,
    SOUND_WALL,
    SOUND_SCORE,
    SOUND_GAME_OVER
} SoundEffect;

void initSound();
void playSound(SoundEffect effect);
void cleanUpSound();

#endif
