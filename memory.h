#ifndef EVAMEM_H
#define EVAMEM_H

#include <stdint.h>
#include <stdio.h>

#define ADDRSPACE 1+
extern uint8_t CONTROL[ADDRSPACE 0xFF];
extern uint8_t EVRAM[ADDRSPACE 0x1FFFF];
extern uint8_t EWRAM[ADDRSPACE 0x1FFFF];
extern uint8_t ESRAM[ADDRSPACE 0xFFF];

#define GETBIT(var, bit)	(((var) >> (bit)) & 1)
#define SETBIT(var, bit)	var |= (1 << (bit))
#define CLRBIT(var, bit)	var &= (~(1 << (bit)))

#define UNMAPPED 0xFFFFFFFF

#define R0 0
#define R1 1
#define R2 2
#define R3 3
#define F0 4
#define F1 5
#define F2 6
#define F3 7

typedef struct
{
	uint32_t start;
	uint32_t end;
} mem_region_t;

void map_region (mem_region_t *mem_region, uint32_t start, uint32_t end);
uint16_t eva_communicate_bus_read (uint32_t location);
void eva_communicate_bus_write (uint32_t location, uint16_t value);
void eva_communicate_bus_write_b (uint32_t location, uint8_t value);

#endif
