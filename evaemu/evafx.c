#include "evafx.h"

evafx_t evafx;

e_long* evafx_set_sprite ( e_byte* vram, e_byte width_in_tiles, e_byte height_in_tiles, e_word tile_start_index ) 
{
    	// Each tile is 32 bytes (8x8 pixels, 4 bits per pixel)
    	const int TILE_SIZE_BYTES = 32;
    
    	// Calculate the total number of tiles
    	int total_tiles = width_in_tiles * height_in_tiles;
    
    	// Allocate memory to store the sprite (4 bytes for each longword in each tile)
    	uint32_t* sprite = (uint32_t*)malloc(total_tiles * 8 * sizeof(uint32_t)); // 8 longwords per tile

    	if (!sprite) {
        	// Memory allocation failed
        	return NULL;
    	}

    	// Copy the tile data from VRAM into the newly allocated array
    	// tile_start_index is the index of the first tile in VRAM
    	for (int i = 0; i < total_tiles; i++) {
        	// Copy 32 bytes from VRAM to the sprite
        	memcpy((uint8_t*)&sprite[i * 8], &vram[(tile_start_index + i) * TILE_SIZE_BYTES], TILE_SIZE_BYTES);
    	}

    	return sprite;
}

void evafx_transfer_sprite_to_vram ( uint32_t* sprite, uint8_t* vram, int width_in_tiles, int height_in_tiles, uint32_t tile_start_index ) 
{
    	// Each tile is 32 bytes (8x8 pixels, 4 bits per pixel)
    	const int TILE_SIZE_BYTES = 32;
    
    	// Calculate the total number of tiles
    	int total_tiles = width_in_tiles * height_in_tiles;

    	// Copy the sprite data back into VRAM
    	for (int i = 0; i < total_tiles; i++) {
        	// Copy 32 bytes from the sprite array to VRAM
        	memcpy(&vram[(tile_start_index + i) * TILE_SIZE_BYTES], (uint8_t*)&sprite[i * 8], TILE_SIZE_BYTES);
    	}
}

void set_pixel_in_sprite ( uint32_t sprite[4][4][8], int x, int y, uint8_t pixelValue ) 
{
	// Ensure pixelValue fits within a nybble (0 to 15)
    	pixelValue &= 0xF;

    	// Determine which tile contains the pixel
    	int tileX = x / 8; // Horizontal tile index (0-3)
    	int tileY = y / 8; // Vertical tile index (0-3)

    	// Determine the position of the pixel within the tile
    	int pixelX = x % 8; // Horizontal position within the 8x8 tile (0-7)
    	int pixelY = y % 8; // Vertical position within the 8x8 tile (0-7)

    	// Calculate the bit position of the nybble in the 32-bit longword
    	int bitPos = pixelX * 4;

    	// Mask to clear the existing nybble in the target longword
    	uint32_t mask = ~(0xF << bitPos);

    	// Set the pixel value in the correct tile and longword
    	sprite[tileY][tileX][pixelY] = ( sprite[tileY][tileX][pixelY] & mask ) | ( pixelValue << bitPos );
}
