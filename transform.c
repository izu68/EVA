#include "transform.h"
#include "bitmap.h"

void linear_transform_sprite
(
	uint32_t evram_location,
	uint8_t width, 
	uint8_t height,
	uint8_t angle_b
)
{
	const int32_t scale = 1 << 16; 	// Fixed-point scaling (2^16)
	int32_t angle = angle_b * (360 / 256); // De-truncate angle range from a byte
	int32_t cos_theta = (int)(cos(angle * M_PI / 180.0) * scale);
	int32_t sin_theta = (int)(sin(angle * M_PI / 180.0) * scale);

	int16_t px_width = width * 8;
	int16_t px_height = height * 8;

	int16_t cx = width / 2;		// Origin x
	int16_t cy = height / 2;	// Origin y
	int16_t dx, dy;			// Origin relative

	int32_t u, v;			// Untransformed pixel coords

        uint8_t pixel_value;

	for (int16_t y = 0; y < px_height; y++)
	{
		for (int16_t x = 0; x < px_width; x++)
		{
			dx = x - cx;
            		dy = y - cy;

            		// Compute rotated coordinates
            		u = ((cos_theta * dx - sin_theta * dy) / scale) + cx;
            		v = ((sin_theta * dx + cos_theta * dy) / scale) + cy;

			// Bounds checking for (u, v)
        		if (u < 0 || u >= width || v < 0 || v >= height)
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
