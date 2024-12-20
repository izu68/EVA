#ifndef POLYGON_H
#define POLYGON_H

#include <stdint.h>

typedef struct
{
	int32_t x, y, z;
} vertex;

typedef struct
{
	vertex v0, v1, v2;
	uint8_t color;
} triangle;
extern triangle polygon_cache[0xFF];

#endif
