#ifndef BITMAP_H
#define BITMAP_H

#include "eva.h"

typedef struct
{
// Most times I only need these ones
	uint32_t absolute_offset;
	uint8_t nybble;
// These ones are part of the calculation, just keeping them here
	uint32_t address_offset;
	uint16_t tile_offset;
	uint16_t byte_offset;
} pixel_pos_t;


void plot_pixel 
(
	uint8_t mode,
	uint32_t evram_location, 
	uint8_t width, 
	uint8_t height, 
	uint8_t x, 
	uint8_t y,
	uint8_t color
);

void read_pixel
(
	uint8_t mode,
	uint32_t evram_location, 
	uint8_t width, 
	uint8_t height, 
	uint8_t x, 
	uint8_t y,
	uint8_t *color
);

#endif
