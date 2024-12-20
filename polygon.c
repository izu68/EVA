#include "polygon.h"
#include "transform.h" // To borrow fixed point macros
#include "bitmap.h"

triangle polygon_cache[0xFF];

// Fixed point math
int32_t fixed_mul (int32_t a, int32_t b)
{
	return (a * b) >> FIXED_SHIFT;
}

int32_t fixed_div (int32_t a, int32_t b)
{
	return (a << FIXED_SHIFT) / b;
}

// Rendering methods
void clear_framebuffer (uint16_t evram_addr, uint8_t fb_width, uint8_t fb_height)
{
	for (int y = 0; y < fb_height; y++) 
	{
        	for (int x = 0; x < fb_width; x++) 
		{
			plot_pixel 
			(
				1,
				evram_addr, 
				fb_width / 8, 
				fb_height / 8, 
				x, 
				y,
				0
			);
        	}
    	}
}

void project_vertex 
(
	const vertex *in, 
	vertex *out, 
	uint8_t fb_width, 
	uint8_t fb_height
)
{
	int32_t fov = FIXED_ONE * 2;
	int32_t z = in->z == 0 ? 1 : in->z; // Prevent div zero
	out->x = fixed_div (in->x, z) * fov + (fb_width / 2) * FIXED_ONE;
	out->x = fixed_div (in->y, z) * fov + (fb_height / 2) * FIXED_ONE;
	out->z = in->z; // Keep Z for depth sorting
}

void plot_horizontal_line 
(
	int x1, 
	int x2, 
	int y, 
	uint8_t color,
	uint16_t evram_addr,
	uint8_t fb_width,
	uint8_t fb_height
)
{
	if (y < 0 || y >= fb_height) return;

    	if (x1 > x2) 
	{
    	    	int temp = x1;
        	x1 = x2;
        	x2 = temp;
    	}

    	if (x2 < 0 || x1 >= fb_width) return;

    	for (int x = x1; x <= x2; x++) 
	{
		plot_pixel 
		(
			1,
			evram_addr, 
			fb_width / 8, 
			fb_height / 8, 
			x, 
			y,
			color
		);
    	}
}

void rasterize_triangle 
(
	const vertex *v0, 
	const vertex *v1, 
	const vertex *v2,
	uint8_t color,
	uint16_t evram_addr,
	uint8_t fb_width,
	uint8_t fb_height
)
{
	const vertex *top = v0;
	const vertex *mid = v1;
	const vertex *bot = v2;
	
	// Sort vertices by y-coordinate
    	if (top->y > mid->y) { const vertex *tmp = top; top = mid; mid = tmp; }
   	if (top->y > bot->y) { const vertex *tmp = top; top = bot; bot = tmp; }
    	if (mid->y > bot->y) { const vertex *tmp = mid; mid = bot; bot = tmp; }

	// Edge slopes
	int32_t dx1 = fixed_div (bot->x - top->x, bot->y - top->y);
    	int32_t dx2 = fixed_div (mid->x - top->x, mid->y - top->y);
    	int32_t dx3 = fixed_div (bot->x - mid->x, bot->y - mid->y);

    	int32_t x1 = top->x;
    	int32_t x2 = top->x;

	// Draw top-half of the triangle
    	for (int y = top->y; y < mid->y; y++) 
	{
        	plot_horizontal_line
		(
			x1 >> FIXED_SHIFT, 
			x2 >> FIXED_SHIFT, 
			y >> FIXED_SHIFT, 
			color,
			evram_addr,
			fb_width,
			fb_height
		);
        	x1 += dx1;
        	x2 += dx2;
    	}

    	// Draw bottom-half of the triangle
    	x2 = mid->x;
    	for (int y = mid->y; y < bot->y; y++) 
	{
        	plot_horizontal_line
		(
			x1 >> FIXED_SHIFT, 
			x2 >> FIXED_SHIFT, 
			y >> FIXED_SHIFT, 
			color,
			evram_addr,
			fb_width,
			fb_height
		);
        	x1 += dx1;
        	x2 += dx3;
    	}
}

void render_triangles 
(
	size_t count, 
	uint16_t evram_addr,
	uint8_t fb_width, 
	uint8_t fb_height
) 
{
    	for (size_t i = 0; i < count; i++) 
	{
        	triangle t = polygon_cache[i];

        	// Backface culling
        	int32_t dx1 = t.v1.x - t.v0.x;
        	int32_t dy1 = t.v1.y - t.v0.y;
        	int32_t dx2 = t.v2.x - t.v0.x;
        	int32_t dy2 = t.v2.y - t.v0.y;
        	int32_t cross = dx1 * dy2 - dy1 * dx2;

        	if (cross > 0) continue; // Skip back-facing triangles

        	vertex v0, v1, v2;
        	project_vertex (&t.v0, &v0, fb_width, fb_height);
        	project_vertex (&t.v1, &v1, fb_width, fb_height);
        	project_vertex (&t.v2, &v2, fb_width, fb_height);

        	rasterize_triangle 
		(
			&v0, 
			&v1,
			&v2, 
			t.color,
			evram_addr,
			fb_width,
			fb_height
		);
    	}
}
