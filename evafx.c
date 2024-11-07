// -------------------------------------------------------------------------------
// EVA - Firmware GFX module
// Contents :
//		Random pixel R/W handler
//		Rot/scale handler
//		GFX instructions
// Notes :
//		*
// -------------------------------------------------------------------------------

#include "evafx.h"

// =============================== HANDLERS ======================================

#define TYPE_TILEMAP		0
#define TYPE_SPRITE	 	1
#define MODE_READ		0
#define MODE_WRITE		1
e_byte access_pixel 
( 
	e_byte mode, e_byte order, 
	e_word vram_offset, 
	e_byte sprite_width, e_byte sprite_height, e_word x, e_word y, 
	e_byte color 
) 
{
	if ( x > sprite_width * 8 ) x = x - ( sprite_width * 8 );
    	// Calculate tile coordinates
   	e_byte tile_x = x / 8;
    	e_byte tile_y = y / 8;

    	// Calculate tile index in VRAM based on object tile order
	e_word tile_index;
	if ( order == TYPE_TILEMAP )
	{
    		tile_index = tile_y * sprite_width + tile_x;
	}
	else
	{
    		tile_index = tile_x * sprite_height + tile_y;
	}

    	// Calculatee pixel position within the tile
    	e_word pixel_x = x % 8;
    	e_word pixel_y = y % 8;

    	e_word tile_offset = tile_index * 32;

    	// Calculate the byte offset within the tile
    	e_word byte_offset = pixel_y * 4 + (pixel_x / 2);
    
    	// Fetch the byte containing the target pixel
    	e_byte byte = EVA_RAM[0x01][vram_offset + tile_offset + byte_offset];

    	// Determine if we're modifying the high or low nybble
	if ( mode == MODE_WRITE )
	{
    		if (pixel_x % 2 == 0) 
		{
        		// Modify high nybble
       			byte = (byte & 0x0F) | (color << 4);
    		} 
		else 
		{
        		// Modify low nybble
        		byte = (byte & 0xF0) | color;
    		}
	}
	else
	{
		/* printf 
		( 
			"read pixel from offset %04X with coords X:%d Y:%d COLOR:%X\n", 
			tile_offset+byte_offset, x, y, byte
		); */
		if (pixel_x % 2 == 0) 
		{
        		// Return high nybble
        		return (byte >> 4) & 0x0F;
    		} 
		else 
		{
        		// Return low nybble
        		return byte & 0x0F;
    		}
	}

    	// Store modified byte back into VRAM
    	EVA_RAM[0x01][tile_offset + byte_offset] = byte;
	return 0;
}

void fast_rotnscale 
( 
	e_word vram_offset, e_word trans_vram_offset, 
	e_byte spr_w, e_byte spr_h, 
	float angle, 
	float x_scale, float y_scale
)
{
	if ( x_scale > 1.0f ) x_scale = 1.0f;
	if ( y_scale > 1.0f ) y_scale = 1.0f;
	int width = spr_w * 8;
	int height = spr_h * 8;
	for ( int y = 0; y < height; y++ )
	{
		for ( int x = 0; x < width; x++ )
		{
			float x_prime = x - (width / 2);
			float y_prime = y - (height / 2);

			float u = (cos(-angle) * x_prime * (1.0f / x_scale) + 
					sin(-angle) * y_prime * (1.0f / x_scale)) + (width / 2);
			float v = (-sin(-angle) * x_prime * (1.0f / y_scale) + 
					cos(-angle) * y_prime * (1.0f / y_scale)) + (height / 2);

			/* float u = ( cos ( -angle ) * x * ( 1.0f / scale )
			    + sin ( -angle ) * y * ( 1.0f / scale ) );
			float v = ( -sin ( -angle ) * x * ( 1.0f / scale )
			    + cos ( -angle ) * y * ( 1.0f / scale ) ); */
			
			access_pixel 
			(
				// Write to sprite
				MODE_WRITE,
				TYPE_SPRITE,
				vram_offset,
				spr_w, spr_h,
				x, y,
				access_pixel
				(
					// Read from sprite
					MODE_READ,
					TYPE_SPRITE,
					trans_vram_offset,
					spr_w, spr_h,
					u, v,
					0
				)
			);
		}
	}
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
	/* printf ( "(EVA) Plot pixel: Offset:%04X W:%02X H:%02X X:%02X Y:%02X Color:%01X\n", 
		 vram_offset, spr_w, spr_h, spr_x, spr_y, color ); */
	access_pixel 
	(
		// Write
		MODE_WRITE,
		// Sprite tile order
		TYPE_SPRITE,
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

float test_angle = 0;
float test_x_scale = 0.0f;
float test_y_scale = 0.0f;
void eva_rgfx ( void )
{
	e_byte angle = EVA_RAM[0x00][eva.pc+1];
	e_word vram_offset = EVA_RAM[0x00][eva.pc+2] << 8 | EVA_RAM[0x00][eva.pc+3];
	e_byte spr_w = EVA_RAM[0x00][eva.pc+4];
	e_byte spr_h = EVA_RAM[0x00][eva.pc+5];
	fast_rotnscale
	(
		vram_offset,
		0x2000,
		spr_w, spr_h,
		test_angle,
		test_x_scale, test_y_scale
	);
	//test_angle += 0.01f;
	test_x_scale += 1.0f;
	test_y_scale += 0.005f;
}
