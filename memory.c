#include "memory.h"
#include "eva.h"

uint8_t CONTROL[ADDRSPACE 0xFF];
uint8_t EVRAM[ADDRSPACE 0xFFFF];
uint8_t TRAM[ADDRSPACE 0xFFFF];
uint8_t EWRAM[ADDRSPACE 0x1FFFF];
uint8_t ESRAM[ADDRSPACE 0xFFF];

void map_region (mem_region_t *mem_region, uint32_t start, uint32_t end)
{
	mem_region->start = start;
	mem_region->end = end;
}

uint8_t evaluate_mem_region (mem_region_t *mem_region, uint32_t location)
{
	if (location >= mem_region->start && location <= mem_region->end)
	{
		return 1;
	}
	return 0;
};

void write_reg (uint8_t reg, uint32_t value)
{
	switch (reg)
	{
		default: printf ("(EVA ERROR) Unsupported register\n"); break;
		case R0: eva.r0 = value; break;
		case R1: eva.r1 = value; break;
		case R2: eva.r2 = value; break;
		case R3: eva.r3 = value; break;
	}
}

void write_freg (uint8_t reg, uint8_t word, uint32_t value)
{
	
}

uint16_t eva_communicate_bus_read (uint32_t location)
{
	// Important note:
	//
	// Only a word sized read handler is present, opposed to the write
	// handlers which come in word and byte size varieties. This is because
	// it seems BlastEm's addressing code already handles byte writes by accessing
	// the individual byte of a word as needed.
	
	if (evaluate_mem_region (&eva.region_evram, location))
	{
		return EVRAM[location - eva.region_evram.start] << 8 | 
		       EVRAM[location - eva.region_evram.start + 1];
	}

	// CONTROL region reads
	// Status register (A13000) is allowed
	if (location >= 0xA13000 && location <= 0xA130E0)
	{
		return CONTROL[location - 0xA13000] << 8 | CONTROL[location - 0xA13000 + 1];
	}

	return 0;
}

void eva_communicate_bus_write (uint32_t location, uint16_t value)
{
	if (evaluate_mem_region (&eva.region_evram, location))
	{
		EVRAM[location - eva.region_evram.start] = value >> 8;
		EVRAM[location - eva.region_evram.start + 1] = value;
	}

	// CONTROL region word writes
	if (location >= 0xA13002 && location <= 0xA130E0)
	{
		CONTROL[location - 0xA13000] = value >> 8;
		CONTROL[location - 0xA13000 + 1] = value;
	}
}

void eva_communicate_bus_write_b (uint32_t location, uint8_t value)
{
	// RAM regions byte writes (CAX)
	if (eva.mmap_mode == CAX)
	{
		if (location >= 0x3F0000 && location <= 0x3FFFFF)
		{	
			EVRAM[location - 0x3F0000] = value;
		}
	}
	
	// CONTROL region byte writes
	// Status register (A13000) is restricted
	if (location >= 0xA13002 && location <= 0xA130E0)
	{
		CONTROL[location - 0xA13000] = value;
	}
}
