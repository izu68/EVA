#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "eva.h"
#include <math.h>

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
);

#endif
