#ifndef PCM_H
#define PCM_H

#include "eva.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_mixer.h>

#define MAX_SOUND_BANKS 256
#define MAX_CHANNELS 16

typedef struct 
{
    	Mix_Chunk* bank;
} sound_cache_entry;

uint8_t init_pcm_system (void);
void load_sound_bank (uint8_t sound_bank);
void play_sound_bank (uint8_t sound_bank, uint8_t channel);
void halt_channel (int8_t channel);
void free_sound_bank (uint8_t sound_bank);

#endif
