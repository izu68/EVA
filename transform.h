#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "eva.h"
#include <math.h>
#include <stdbool.h>

#define FIXED_SHIFT 16
#define FIXED_ONE (1 << FIXED_SHIFT)

typedef struct
{
	uint16_t evram_location;
	uint8_t width;
	uint8_t height;
	bool hflip;
	bool vflip;
} gfx_cache_t;

void write_gfx_cache 
(
	uint8_t cache_index, 
	uint16_t tile_index, 
	uint8_t width, 
	uint8_t height, 
	bool hflip, 
	bool vflip
);

void transform_sprite
(
	uint8_t cache_index,
	uint8_t origin_x,
	uint8_t origin_y,
	uint8_t angle_b,
	uint8_t scale_x_b,
	uint8_t scale_y_b,
	uint8_t fill_color
);

void transform_plane
(
	uint8_t cache_index,
	uint8_t origin_x,
	uint8_t origin_y,
	uint8_t angle_b,
	uint8_t scale_x_b,
	uint8_t scale_y_b,
	uint8_t fill_color
);

#endif
