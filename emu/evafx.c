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
#define MODE_READ_TRAM		2
e_byte access_pixel 
( 
	e_byte mode, e_byte order, 
	e_word vram_offset, 
	e_byte sprite_width, e_byte sprite_height, e_word x, e_word y, 
	e_byte color 
) 
{
	if (x > sprite_width * 8) x = x - (sprite_width * 8);
    	// Calculate tile coordinates
   	e_byte tile_x = x / 8;
    	e_byte tile_y = y / 8;

    	// Calculate tile index in VRAM based on object tile order
	e_word tile_index;
	if (order == TYPE_TILEMAP)
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
    
	// Fetch or write data accordingly
	if (mode == MODE_WRITE)
	{
    		e_byte byte = EVA_VRAM[vram_offset + tile_offset + byte_offset];
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
    		// Store modified byte back into VRAM
    		EVA_VRAM[tile_offset + byte_offset] = byte;
		return 0;
	}
	else if (mode == MODE_READ)
	{
    		e_byte byte = EVA_VRAM[vram_offset + tile_offset + byte_offset];
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
	else if (mode == MODE_READ_TRAM)
	{
    		e_byte byte = EVA_TRAM[vram_offset + tile_offset + byte_offset];
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

	return 0;
}

void fast_rotnscale 
(
	e_byte type,
	e_word vram_offset, 
	e_byte w, e_byte h, 
	float angle_deg, 
	float x_scale, float y_scale
)
{
	int width = w * 8;
	int height = h * 8;
	float angle = angle_deg * (M_PI / 180); // DEG -> RAD

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float x_prime = x - (width / 2);
			float y_prime = y - (height / 2);

			float u = (cos(-angle) * x_prime * (1.0f / x_scale) + 
					sin(-angle) * y_prime * (1.0f / x_scale)) + (width / 2);
			float v = (-sin(-angle) * x_prime * (1.0f / y_scale) + 
					cos(-angle) * y_prime * (1.0f / y_scale)) + (height / 2);

			// Bounds checking for u, v
        		int pixel_value;
        		if (u < 0 || u >= width || v < 0 || v >= height)
        		{
           			 pixel_value = 0;
       			}
        		else
        		{
            			pixel_value = access_pixel
            			(
                			MODE_READ_TRAM,
                			type,
                			vram_offset,
                			w, h,
                			u, v,
                			0
            			);
        		}

        		// Write to transformed
        		access_pixel
        		(
            			MODE_WRITE,
            			type,
            			vram_offset,
            			w, h,
            			x, y,
            			pixel_value
        		);
		}
	}
}

// =============================== INSTRUCTIONS ==================================

void eva_pps (void)
{
	e_byte color = EVA_CONTROL[eva.pc+1];
	e_word vram_offset = EVA_CONTROL[eva.pc+2] << 8 | EVA_CONTROL[eva.pc+3];
	e_byte spr_w = EVA_CONTROL[eva.pc+4];
	e_byte spr_h = EVA_CONTROL[eva.pc+5];
	e_byte spr_x = EVA_CONTROL[eva.pc+8];
	e_byte spr_y = EVA_CONTROL[eva.pc+9];
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

void eva_uptram (void)
{
	for (e_word i = 0; i < 0xFFFF; i++)
	{
		EVA_TRAM[i] = EVA_VRAM[i];
	}
}

void eva_ltsp (void)
{
	float angle = (EVA_CONTROL[eva.pc + 1] * 1.40625f);
	e_word vram_offset = EVA_CONTROL[eva.pc + 2] << 8 | EVA_CONTROL[eva.pc + 3];
	e_byte spr_w = EVA_CONTROL[eva.pc + 4];
	e_byte spr_h = EVA_CONTROL[eva.pc + 5];
	float x_scale = (EVA_CONTROL[eva.pc + 6] / 255.0f);
	float y_scale = (EVA_CONTROL[eva.pc + 7] / 255.0f);
	fast_rotnscale
	(
		TYPE_SPRITE,
		vram_offset,
		spr_w, spr_h,
		angle,
		x_scale, y_scale
	);
}
