#include "transform.h"
#include "bitmap.h"

void linear_transform_sprite
(
	uint32_t evram_location,
	uint8_t width, 
	uint8_t height,
	uint8_t angle_b,
	float x_scale_b, 
	float y_scale_b
)
{
	int angle_deg = angle_b * (360 / 256);
	float angle = angle_deg * (M_PI / 180); // DEG -> RAD

	int px_width = width * 8;
	int px_height = height * 8;

	int x_scale = x_scale_b / 256;
	int y_scale = y_scale_b / 256;

	for (int y = 0; y < px_height; y++)
	{
		for (int x = 0; x < px_width; x++)
		{
			float x_prime = x - (px_width / 2);
			float y_prime = y - (px_height / 2);

			float u = (cos(-angle) * x_prime * (1.0f / x_scale) +
					sin(-angle) * y_prime * (1.0f / x_scale)) + (px_width / 2);
			float v = (-sin(-angle) * x_prime * (1.0f / y_scale) +
					cos(-angle) * y_prime * (1.0f / y_scale)) + (px_height / 2);

			// Bounds checking for u, v
        		uint8_t pixel_value;
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
