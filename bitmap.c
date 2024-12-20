#include "bitmap.h"


static void compute_pixel_position
(
    	uint8_t mode,	// 0 = sprite mode, !0 = bg tilemap mode
    	pixel_pos_t *pixel_pos,
    	uint32_t address_offset,
    	uint8_t width,
    	uint8_t height,
    	uint16_t x,
    	uint16_t y
)
{
    	pixel_pos->address_offset = address_offset;

	uint16_t pixel_x, pixel_y;
	if (mode == 0)
	{
		// Column-major order for sprites
    		// Vertical slice handling for height > 32 pixels
    		uint8_t slice_index = y / 32;  // Determine which 32-pixel slice we're in
    		uint8_t slice_y = y % 32;      // Local y within the current 32-pixel slice

    		uint8_t tile_x = x / 8;
    		uint8_t tile_y = slice_y / 8;

    		// Compute the base tile offset for the current slice
    		uint16_t tile_index = tile_x * 4 + tile_y;
    		pixel_pos->tile_offset = tile_index * 32;

    		// Add the contribution of previous slices
    		pixel_pos->tile_offset += slice_index * (width * 4 * 32);

    		pixel_x = x % 8;
    		pixel_y = slice_y % 8;
	}
	else
	{
		// Row-major order for bg tilemaps
		uint8_t tile_x = x / 8;
		uint8_t tile_y = y / 8;

		uint16_t tile_index = tile_y * width + tile_x;
		pixel_pos->tile_offset = tile_index * 32;

		pixel_x = x % 8;
		pixel_y = y % 8;		
	}

    	// Offsets
    	pixel_pos->byte_offset = pixel_y * 4 + (pixel_x / 2);
    	pixel_pos->nybble = (pixel_x % 2 == 0) ? 0 : 1;
    	pixel_pos->absolute_offset = pixel_pos->address_offset +
                                 pixel_pos->tile_offset +
                                 pixel_pos->byte_offset;
}

void plot_pixel
(
	uint8_t mode,
	uint32_t evram_location, 
	uint8_t width, 
	uint8_t height, 
	uint8_t x, 
	uint8_t y,
	uint8_t color
)
{
	pixel_pos_t pixel_pos;
	compute_pixel_position (mode, &pixel_pos, evram_location, width, height, x, y);
	// Get original byte to preserve potential pixels that were already
	// in the high or low nybble
    	uint8_t byte = EVRAM[pixel_pos.absolute_offset];
	if (pixel_pos.nybble == 0)
	{	
       		byte = (byte & 0x0F) | (color << 4);
    	}
	else
	{
        	byte = (byte & 0xF0) | color;
    	}
    	// Store modified byte back
    	EVRAM[pixel_pos.absolute_offset] = byte;
}

void read_pixel
(
	uint8_t mode,
	uint32_t evram_location, 
	uint8_t width, 
	uint8_t height, 
	uint8_t x, 
	uint8_t y,
	uint8_t *color
)
{
	pixel_pos_t pixel_pos;
	compute_pixel_position (mode, &pixel_pos, evram_location, width, height, x, y);
    	uint8_t byte = EVRAM[pixel_pos.absolute_offset];
	if (pixel_pos.nybble == 0)
	{
        	*color = (byte >> 4) & 0x0F;
    	}
	else
	{
        	*color = byte & 0x0F;
    	}
}
