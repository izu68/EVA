#include "memory.h"
#include "eva.h"

uint8_t CONTROL[ADDRSPACE 0xFF];
uint8_t EVRAM[ADDRSPACE 0xFFFF];
uint8_t EWRAM[ADDRSPACE 0x1FFFF];
uint8_t ESRAM[ADDRSPACE 0xFFF];

// TODO:
// Work in the register write logic with commands in the CT.
// Work in ESRAM logic.
// Work in floating point conversion logic for registers.

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
	// Note:
	//
	// Only a word sized read handler is present, opposed to the write
	// handlers which come in word and byte size varieties. This is because
	// it seems BlastEm's addressing code already handles byte reads using
	// the word read handler and taking the MSB.
	
	if (evaluate_mem_region (&eva.region_evram, location))
	{
		return EVRAM[location - eva.region_evram.start] << 8 | 
		       EVRAM[location - eva.region_evram.start + 1];
	}
	else if (evaluate_mem_region (&eva.region_ewram, location))
	{
		return EWRAM[location - eva.region_ewram.start] << 8 |
		       EWRAM[location - eva.region_ewram.start + 1];	
	}

	// CONTROL region reads
	if (location >= 0xA13010 && location <= 0xA130E0)
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

		EVRAM[0x8000 + location - eva.region_evram.start] = value >> 8;
		EVRAM[0x8000 + location - eva.region_evram.start + 1] = value;
	}
	else if (evaluate_mem_region (&eva.region_ewram, location))
	{
		EWRAM[location - eva.region_ewram.start] = value >> 8;
		EWRAM[location - eva.region_ewram.start + 1] = value;	
	}

	// CONTROL region word writes
	// System Status register (A13011) is restricted
	if (location >= 0xA13010 && location <= 0xA130E0)
	{
		if (location == 0xA13011) return;
		CONTROL[location - 0xA13000] = value >> 8;
		CONTROL[location - 0xA13000 + 1] = value;
		if (location >= 0xA13012 && location <= 0xA13015) listen_startup_magic ();
		if (location == 0xA130B0) trigger_command_table ();
	}
}

void eva_communicate_bus_write_b (uint32_t location, uint8_t value)
{
	if (evaluate_mem_region (&eva.region_evram, location))
	{
		EVRAM[location - eva.region_evram.start] = value;
		EVRAM[0x8000 + location - eva.region_evram.start] = value;
	}
	if (evaluate_mem_region (&eva.region_ewram, location))
	{	
		EWRAM[location - eva.region_ewram.start] = value;
	}
	
	// CONTROL region byte writes
	// System Status register (A13011) is restricted
	if (location >= 0xA13010 && location <= 0xA130E0)
	{
		if (location == 0xA13011) return;
		CONTROL[location - 0xA13000] = value;
		if (location >= 0xA13012 && location <= 0xA13015) listen_startup_magic ();
		if (location == 0xA130B0) trigger_command_table ();
	}
}
