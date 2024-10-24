#ifndef H_EVAFX
#define H_EVAFX

#include <stdint.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include "eva.h"

typedef struct
{
	e_long *spr;
	bool used;
} sprite_t;

typedef struct
{
	sprite_t sprite[ADDRSPACE 0xFF];
} evafx_t;
extern evafx_t evafx;

e_long* evafx_set_sprite ( e_byte* vram, e_byte width_in_tiles, e_byte height_in_tiles, e_word tile_start_index );
 
#endif
