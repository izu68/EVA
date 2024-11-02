#include "evafx.h"

// =============================== HANDLERS ======================================

/* void plot_sprite_pixel 
( 
	e_word vram_offset, e_byte width_in_tiles, 
	e_byte height_in_tiles, e_word x, e_word y, e_byte color
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
        	EVA_RAM[0x01][vram_offset+byte_index] = 
		( EVA_RAM[0x01][vram_offset+byte_index] & 0x0F ) | ( color << 4 );
    	} 
	else 
	{
        	// Write to the low nybble
        	EVA_RAM[0x01][vram_offset+byte_index] = 
		( EVA_RAM[0x01][vram_offset+byte_index] & 0xF0 ) | ( color & 0x0F );
    	}
} */

#define PIXEL_TILE_ORDER 	0
#define PIXEL_SPRITE_ORDER 	1
#define MODE_READ		0
#define MODE_WRITE		1
void access_pixel 
( 
	e_byte mode, e_byte order, 
	e_word vram_offset, 
	e_byte sprite_width, e_byte sprite_height, e_word x, e_word y, 
	e_byte color 
) 
{
    	// Calculate the tile coordinates
   	int tile_x = x / 8;
    	int tile_y = y / 8;

    	// Calculate the tile index in VRAM based on the sprite width in tiles
    	int tile_index = tile_x * sprite_height + tile_y;

    	// Calculate the pixel position within the tile
    	int pixel_x = x % 8;
    	int pixel_y = y % 8;

    	// Each tile is 32 bytes (8x8 pixels, 4 bits per pixel)
    	int tile_offset = tile_index * 32;

    	// Calculate the byte offset within the tile
    	int byte_offset = pixel_y * 4 + (pixel_x / 2);
    
    	// Fetch the byte containing the target pixel
    	uint8_t byte = EVA_RAM[0x01][tile_offset + byte_offset];

    	// Determine if we're modifying the high or low nybble
    	if (pixel_x % 2 == 0) 
	{
        	// Modify the high nybble
       		byte = (byte & 0x0F) | (color << 4);
    	} 
	else 
	{
        	// Modify the low nybble
        	byte = (byte & 0xF0) | color;
    	}

    	// Store the modified byte back into VRAM
    	EVA_RAM[0x01][tile_offset + byte_offset] = byte;
}

// =============================== INSTRUCTIONS ==================================

void eva_pps ( void )
{
	e_byte color = EVA_RAM[0x00][eva.pc+1];
	e_word vram_offset = EVA_RAM[0x00][eva.pc+2] << 8 | EVA_RAM[0x00][eva.pc+3];
	e_byte spr_w = EVA_RAM[0x00][eva.pc+4];
	e_byte spr_h = EVA_RAM[0x00][eva.pc+5];
	e_byte spr_x = EVA_RAM[0x00][eva.pc+8];
	e_byte spr_y = EVA_RAM[0x00][eva.pc+9];
	printf ( "(EVA) Plot pixel: Offset:%04X W:%02X H:%02X X:%02X Y:%02X Color:%01X\n", 
		 vram_offset, spr_w, spr_h, spr_x, spr_y, color );
	access_pixel 
	(
		// Write
		MODE_WRITE,
		// Sprite tile order
		PIXEL_SPRITE_ORDER,
		// Offset in VRAM (ECT1E)
		vram_offset,
		// SPR width and height (ECT1)
		spr_w, spr_h,
		// SPR X and Y (ECT2)
		spr_x, spr_y,
		// Pixel value (ECT0E)
		color
	);
}
