#include "transform.h"
#include "bitmap.h"

/*
void linear_transform_sprite
(
	uint32_t evram_location,
	uint8_t width, 
	uint8_t height,
	uint8_t origin_x,
	uint8_t origin_y,
	uint8_t angle_b
)
{
	const int32_t scale = 1 << 16; 	// Fixed-point scaling (2^16)
	int32_t angle = angle_b * 1.40625; // De-truncate angle range from a byte (256 = 360DEG)
	int32_t cos_theta = (int)(cos(angle * M_PI / 180.0) * scale);
	int32_t sin_theta = (int)(sin(angle * M_PI / 180.0) * scale);

	int16_t px_width = width * 8;
	int16_t px_height = height * 8;

	int16_t dx, dy;			// Origin relative

	int32_t u, v;			// Untransformed pixel coords

        uint8_t pixel_value;

	for (int16_t y = 0; y < px_height; y++)
	{
		for (int16_t x = 0; x < px_width; x++)
		{
			dx = x - origin_x;
            		dy = y - origin_y;

            		// Compute rotated coordinates
            		u = ((cos_theta * dx - sin_theta * dy) / scale) + origin_x;
            		v = ((sin_theta * dx + cos_theta * dy) / scale) + origin_y;

			// Bounds checking for (u, v)
        		if (u < 0 || u >= px_width || v < 0 || v >= px_height)
        		{
           			 pixel_value = 0;
       			}
        		else
        		{
				// Read from untransformed
				read_pixel
				(
					0,
					evram_location + 0x8000, 
					width, 
					height, 
					u, 
					v,
					&pixel_value
            			);
        		}

        		// Write to transformed
			plot_pixel
			(
				0,
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
*/

void linear_transform_sprite
(
	uint32_t evram_location,
	uint8_t width,
	uint8_t height,
	uint8_t origin_x,
	uint8_t origin_y,
	uint8_t angle_b,
	uint8_t scale_x_b,
	uint8_t scale_y_b
)
{
	const int32_t scale = 1 << 16; 	     	// Fixed-point scaling (2^16)
	int32_t angle = angle_b * 1.40625;   	// 1.40625 = 360 / 256

	int32_t cos_theta = (int32_t)(cos(angle * M_PI / 180.0) * scale);
	int32_t sin_theta = (int32_t)(sin(angle * M_PI / 180.0) * scale);

	if (scale_x_b == 0) scale_x_b = 1;	// Just in case hehe
	if (scale_y_b == 0) scale_y_b = 1;
	//int32_t scale_x = (int32_t)((scale * scale_x_b) / 255);
	//int32_t scale_y = (int32_t)((scale * scale_y_b) / 255);
	int32_t scale_x = 255 / scale_x_b;
	int32_t scale_y = 255 / scale_y_b;

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

			scaled_dx = dx / scale_x;
			scaled_dy = dy / scale_y;

			// Compute rotated coordinates
			u = ((cos_theta * scaled_dx - sin_theta * scaled_dy) / scale) + origin_x;
			v = ((sin_theta * scaled_dx + cos_theta * scaled_dy) / scale) + origin_y;

			// Bounds checking for (u, v)
			if (u < 0 || u >= px_width || v < 0 || v >= px_height)
			{
				pixel_value = 0; // Transparent or blank pixel
			} 
			else 
			{
				// Read pixel from untransformed sprite
				read_pixel
				(
					0,
					evram_location + 0x8000,
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
				0,
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

