#include "transform.h"
#include "bitmap.h"

gfx_cache_t gfx_cache[0xFF];

void write_gfx_cache 
(
	uint8_t cache_index, 
	uint16_t tile_index, 
	uint8_t width, 
	uint8_t height, 
	bool hflip, 
	bool vflip
)
{
	uint32_t evram_location = tile_index * 32;
	gfx_cache[cache_index].evram_location = evram_location;
	gfx_cache[cache_index].width = width;
	gfx_cache[cache_index].height = height;
	gfx_cache[cache_index].hflip = hflip;
	gfx_cache[cache_index].vflip = vflip;
}

static void transform
(
	uint8_t mode,
	uint8_t cache_index,
	uint8_t origin_x,
	uint8_t origin_y,
	uint8_t angle_b,
	uint8_t scale_x_b,
	uint8_t scale_y_b,
	uint8_t fill_color
)
{
	uint32_t evram_location = gfx_cache[cache_index].evram_location;
	uint8_t width = gfx_cache[cache_index].width;
	uint8_t height = gfx_cache[cache_index].height;

	const int32_t scale = FIXED_ONE; 	// Fixed-point scaling (2^16)
	int32_t angle = angle_b * 1.40625;   	// 1.40625 = 360 / 256

	int32_t cos_theta = (int32_t)(cos(angle * M_PI / 180.0) * scale);
	int32_t sin_theta = (int32_t)(sin(angle * M_PI / 180.0) * scale);
	
	scale_x_b = scale_x_b == 0 ? 1 : scale_x_b;	// Prevent div zero
	scale_y_b = scale_y_b == 0 ? 1 : scale_y_b;	// Prevent div zero
	int32_t scale_x = (scale * scale_x_b) / 255;
	int32_t scale_y = (scale * scale_y_b) / 255;

	int16_t px_width = width * 8; 
	int16_t px_height = height * 8;

	int32_t scaled_dx, scaled_dy, dx, dy;	// Origin-relative coordinates
	int32_t u, v;				// Transformed pixel coordinates
	uint8_t pixel_value;

	for (int16_t y = 0; y < px_height; y++) 
	{
		for (int16_t x = 0; x < px_width; x++) 
		{
			dx = x - origin_x;
			dy = y - origin_y;

			scaled_dx = (dx * scale) / scale_x;
			scaled_dy = (dy * scale) / scale_y;

			// Compute rotated coordinates
			u = ((cos_theta * scaled_dx - sin_theta * scaled_dy) / scale) + origin_x;
			v = ((sin_theta * scaled_dx + cos_theta * scaled_dy) / scale) + origin_y;

			// Bounds checking for (u, v)
			if (u < 0 || u >= px_width || v < 0 || v >= px_height)
			{
				pixel_value = fill_color;
			} 
			else 
			{
				// Read pixel from untransformed sprite
				if (gfx_cache[cache_index].hflip) u = px_width - u;
				if (gfx_cache[cache_index].vflip) v = px_height - v;
				read_pixel
				(
					mode,
					evram_location + 0x10000,
					width,
					height,
					u,
					v,
					&pixel_value
				);
			}

			// Write transformed pixel to destination
			plot_pixel
			(
				mode,
				evram_location,
				width,
				height,
				x,
				y,
				pixel_value
			);
		}
	}
}

void transform_sprite
(
	uint8_t cache_index,
	uint8_t origin_x,
	uint8_t origin_y,
	uint8_t angle_b,
	uint8_t scale_x_b,
	uint8_t scale_y_b,
	uint8_t fill_color
)
{
	transform 
	(
		0, 
		cache_index, 
		origin_x, 
		origin_y, 
		angle_b, 
		scale_x_b, 
		scale_y_b, 
		fill_color
	);
}

void transform_plane
(
	uint8_t cache_index,
	uint8_t origin_x,
	uint8_t origin_y,
	uint8_t angle_b,
	uint8_t scale_x_b,
	uint8_t scale_y_b,
	uint8_t fill_color
)
{
	transform 
	(
		1, 
		cache_index, 
		origin_x, 
		origin_y, 
		angle_b, 
		scale_x_b, 
		scale_y_b, 
		fill_color
	);
}
