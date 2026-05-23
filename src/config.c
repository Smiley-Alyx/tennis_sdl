#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_PATH "config.cfg"

static Config config = {
    1,
    1,
    1,
    1,
    10,
    SDL_SCANCODE_W,
    SDL_SCANCODE_S,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_DOWN
};

static int parseBool(const char* value) {
    return strcmp(value, "on") == 0 || strcmp(value, "1") == 0;
}

static int clampDifficulty(int value) {
    if (value < 0) return 0;
    if (value > 2) return 2;
    return value;
}

static int normalizeTargetScore(int value) {
    if (value == 5 || value == 10 || value == 15) return value;
    return 10;
}

static SDL_Scancode parseScancode(const char* value, SDL_Scancode fallback) {
    SDL_Scancode scancode = SDL_GetScancodeFromName(value);
    return scancode == SDL_SCANCODE_UNKNOWN ? fallback : scancode;
}

Config* getConfig() {
    return &config;
}

void loadConfig() {
    FILE* file = fopen(CONFIG_PATH, "r");
    if (!file) {
        saveConfig();
        return;
    }

    char key[64];
    char value[64];
    int musicConfigured = 0;

    while (fscanf(file, " %63[^=]=%63s", key, value) == 2) {
        if (strcmp(key, "players") == 0) {
            int players = atoi(value);
            config.playerCount = players == 2 ? 2 : 1;
        } else if (strcmp(key, "difficulty") == 0) {
            config.difficulty = clampDifficulty(atoi(value));
        } else if (strcmp(key, "music") == 0) {
            config.musicEnabled = parseBool(value);
            musicConfigured = 1;
        } else if (strcmp(key, "sound") == 0) {
            config.soundEnabled = parseBool(value);
            if (!musicConfigured) {
                config.musicEnabled = config.soundEnabled;
            }
        } else if (strcmp(key, "target_score") == 0) {
            config.targetScore = normalizeTargetScore(atoi(value));
        } else if (strcmp(key, "player_up") == 0) {
            config.playerUp = parseScancode(value, config.playerUp);
        } else if (strcmp(key, "player_down") == 0) {
            config.playerDown = parseScancode(value, config.playerDown);
        } else if (strcmp(key, "second_player_up") == 0) {
            config.secondPlayerUp = parseScancode(value, config.secondPlayerUp);
        } else if (strcmp(key, "second_player_down") == 0) {
            config.secondPlayerDown = parseScancode(value, config.secondPlayerDown);
        }
    }

    fclose(file);
}

void saveConfig() {
    FILE* file = fopen(CONFIG_PATH, "w");
    if (!file) return;

    fprintf(file, "players=%d\n", config.playerCount);
    fprintf(file, "difficulty=%d\n", config.difficulty);
    fprintf(file, "music=%s\n", config.musicEnabled ? "on" : "off");
    fprintf(file, "sound=%s\n", config.soundEnabled ? "on" : "off");
    fprintf(file, "target_score=%d\n", config.targetScore);
    fprintf(file, "player_up=%s\n", SDL_GetScancodeName(config.playerUp));
    fprintf(file, "player_down=%s\n", SDL_GetScancodeName(config.playerDown));
    fprintf(file, "second_player_up=%s\n", SDL_GetScancodeName(config.secondPlayerUp));
    fprintf(file, "second_player_down=%s\n", SDL_GetScancodeName(config.secondPlayerDown));

    fclose(file);
}
