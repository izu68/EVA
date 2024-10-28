#include "evafx.h"

void plot_sprite_pixel 
( 
	uint8_t *sprite_data, int width_in_tiles, 
	int height_in_tiles, int x, int y, uint8_t color
) 
{
    	// Calculate the tile coordinates and pixel position within the tile
    	int tile_x = x / 8;
    	int tile_y = y / 8;
    	int pixel_x = x % 8;
    	int pixel_y = y % 8;

    	// Calculate the index of the tile in the sprite data array
    	int tile_index = tile_y * width_in_tiles + tile_x;
    
    	// Each tile is 32 bytes (64 pixels, 2 pixels per byte)
    	int tile_offset = tile_index * 32;

    	// Calculate the byte and nybble position for the specific pixel within the tile
    	int pixel_index_in_tile = pixel_y * 8 + pixel_x;
    	int byte_index = tile_offset + ( pixel_index_in_tile / 2 );
    	int is_high_nybble = ( pixel_index_in_tile % 2 == 0 );

    	// Write the color to the appropriate nybble
    	if ( is_high_nybble ) 
	{
        	// Write to the high nybble
        	sprite_data[byte_index] = ( sprite_data[byte_index] & 0x0F ) | ( color << 4 );
    	} 
	else 
	{
        	// Write to the low nybble
        	sprite_data[byte_index] = ( sprite_data[byte_index] & 0xF0 ) | ( color & 0x0F );
    	}
}

