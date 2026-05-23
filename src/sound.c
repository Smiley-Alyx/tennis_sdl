#include "sound.h"
#include "config.h"
#include <SDL2/SDL.h>
#include <stdint.h>

#define SAMPLE_RATE 44100
#define MAX_TONES 16
#define MUSIC_VOLUME 1200
#define MUSIC_LOOP_LENGTH 8

typedef struct {
    float frequency;
    int samplesLeft;
    int totalSamples;
    int volume;
} Tone;

typedef struct {
    float frequency;
    int durationMs;
} MusicNote;

static SDL_AudioDeviceID audioDevice = 0;
static Tone tones[MAX_TONES];
static int toneCount = 0;
static float phase = 0.0f;
static float musicPhase = 0.0f;
static int musicNoteIndex = 0;
static int musicSamplesLeft = 0;

static const MusicNote musicLoop[MUSIC_LOOP_LENGTH] = {
    {392.0f, 180},
    {493.9f, 180},
    {587.3f, 180},
    {493.9f, 180},
    {440.0f, 180},
    {523.3f, 180},
    {659.3f, 220},
    {0.0f, 140}
};

static void enqueueTone(float frequency, int durationMs, int volume) {
    if (toneCount >= MAX_TONES) return;

    Tone* tone = &tones[toneCount++];
    tone->frequency = frequency;
    tone->samplesLeft = SAMPLE_RATE * durationMs / 1000;
    tone->totalSamples = tone->samplesLeft;
    tone->volume = volume;
}

static void popTone() {
    for (int i = 1; i < toneCount; i++) {
        tones[i - 1] = tones[i];
    }
    toneCount--;
    phase = 0.0f;
}

static int16_t nextSample() {
    if (toneCount == 0) return 0;

    Tone* tone = &tones[0];
    float progress = (float)tone->samplesLeft / (float)tone->totalSamples;
    int envelope = (int)(tone->volume * progress);
    int16_t sample = phase < 0.5f ? envelope : -envelope;

    phase += tone->frequency / SAMPLE_RATE;
    if (phase >= 1.0f) phase -= 1.0f;

    tone->samplesLeft--;
    if (tone->samplesLeft <= 0) popTone();

    return sample;
}

static int16_t nextMusicSample() {
    if (musicSamplesLeft <= 0) {
        MusicNote note = musicLoop[musicNoteIndex];
        musicSamplesLeft = SAMPLE_RATE * note.durationMs / 1000;
        musicNoteIndex = (musicNoteIndex + 1) % MUSIC_LOOP_LENGTH;
        musicPhase = 0.0f;
    }

    MusicNote note = musicLoop[
        (musicNoteIndex + MUSIC_LOOP_LENGTH - 1) % MUSIC_LOOP_LENGTH
    ];
    musicSamplesLeft--;

    if (note.frequency <= 0.0f) return 0;

    int16_t sample = musicPhase < 0.5f ? MUSIC_VOLUME : -MUSIC_VOLUME;

    musicPhase += note.frequency / SAMPLE_RATE;
    if (musicPhase >= 1.0f) musicPhase -= 1.0f;

    return sample;
}

static int16_t clampSample(int sample) {
    if (sample > INT16_MAX) return INT16_MAX;
    if (sample < INT16_MIN) return INT16_MIN;
    return (int16_t)sample;
}

static void audioCallback(void* userdata, Uint8* stream, int len) {
    (void)userdata;

    int16_t* buffer = (int16_t*)stream;
    int sampleCount = len / (int)sizeof(int16_t);

    for (int i = 0; i < sampleCount; i++) {
        if (!getConfig()->soundEnabled) {
            buffer[i] = 0;
            continue;
        }
        buffer[i] = clampSample(nextSample() + nextMusicSample());
    }
}

void initSound() {
    SDL_AudioSpec wanted;
    SDL_AudioSpec obtained;

    SDL_zero(wanted);
    wanted.freq = SAMPLE_RATE;
    wanted.format = AUDIO_S16SYS;
    wanted.channels = 1;
    wanted.samples = 512;
    wanted.callback = audioCallback;

    audioDevice = SDL_OpenAudioDevice(NULL, 0, &wanted, &obtained, 0);
    if (audioDevice == 0) return;

    SDL_PauseAudioDevice(audioDevice, 0);
}

void playSound(SoundEffect effect) {
    if (audioDevice == 0) return;
    if (!getConfig()->soundEnabled) return;

    SDL_LockAudioDevice(audioDevice);

    switch (effect) {
        case SOUND_MENU:
            enqueueTone(1046.5f, 25, 6500);
            break;
        case SOUND_PADDLE:
            enqueueTone(880.0f, 45, 9000);
            break;
        case SOUND_WALL:
            enqueueTone(659.3f, 35, 6500);
            break;
        case SOUND_SCORE:
            enqueueTone(220.0f, 120, 8500);
            enqueueTone(164.8f, 120, 8500);
            break;
        case SOUND_GAME_OVER:
            enqueueTone(392.0f, 80, 8500);
            enqueueTone(329.6f, 80, 8500);
            enqueueTone(261.6f, 160, 8500);
            break;
    }

    SDL_UnlockAudioDevice(audioDevice);
}

void cleanUpSound() {
    if (audioDevice == 0) return;

    SDL_CloseAudioDevice(audioDevice);
    audioDevice = 0;
    toneCount = 0;
    musicSamplesLeft = 0;
    musicNoteIndex = 0;
}
