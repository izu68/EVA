#include "eva.h"
#include <time.h>

eva_t eva;

#define CYCLE_TIME_NS 2083 // Simulate ~480 MHz clock (2.083 ns per cycle)

// Note
// This is here for when I compensate the speed difference between the host
// computer running this logic and the STM32H7. 
//
// TODO: Study the cycle amount of each process and insert the waits accordingly
void aprox_clock (int cycles)
{
	long nanoseconds = cycles * CYCLE_TIME_NS;
	struct timespec ts;
    	ts.tv_sec = nanoseconds / 1000000000;
    	ts.tv_nsec = nanoseconds % 1000000000;
    	nanosleep (&ts, NULL);
}

static void halt_68k (void) 
{}

static void release_68k (void)
{}

void eva_init (uint8_t mmap_mode)
{
	printf ("(EVA) System startup requested\n");

	// Clear control
	for (int i = 0; i <= 0xFF; i++)
	{
		CONTROL[i] = 0;
	}
	printf ("(EVA) Control memory cleared\n");

	// Set memory map mode
	switch (mmap_mode)
	{
		default: printf ("(EVA ERROR) Invalid memory map mode, abort init\n"); return; break;
		case 'C': printf ("(EVA) Memory map mode set to CAX\n"); eva.mmap_mode = CAX; break;
		case 'R': printf ("(EVA) Memory map mode set to RAX\n"); eva.mmap_mode = RAX; break;
		case 'X': printf ("(EVA) Memory map mode set to XAX\n"); eva.mmap_mode = XAX; break;
	}
	// Map regions
	switch (eva.mmap_mode)
	{
		case CAX: map_region (&eva.region_evram, 0x3F0000, 0x3FFFFF);
			  map_region (&eva.region_ewram, UNMAPPED, UNMAPPED);
			  map_region (&eva.region_esram, UNMAPPED, UNMAPPED); 
			  break;
		case RAX: map_region (&eva.region_evram, 0x3F0000, 0x3FFFFF);
			  map_region (&eva.region_ewram, 0x3D0000, 0x3EFFFF);
			  map_region (&eva.region_esram, UNMAPPED, UNMAPPED);
			  break;
		case XAX: map_region (&eva.region_evram, 0x7F0000, 0x7FFFFF);
			  map_region (&eva.region_ewram, 0x7D0000, 0x7EFFFF);
			  map_region (&eva.region_esram, 0x7CF000, 0x7CFFFF);
			  break;
	}	

	// Clear registers
	eva.r0 = eva.r1 = eva.r2 = eva.r3 = 0;
	eva.f0 = eva.f1 = eva.f2 = eva.f3 = 0;
	eva.cc = 10;
	eva.s0 = 0;
	eva.addr = 0;
	eva.data = 0;
	printf ("(EVA) Registers initialized\n");

	// Status register initialization
	SETBIT (eva.s0, 0xF);	// System ready
}

void listen_startup_magic (void)
{
	if (CONTROL[0x02] == 'E' && CONTROL[0x03] == 'V' && CONTROL[0x04] == 'A')
	{
		eva_init (CONTROL[0x05]);	// Init according to specified memory map mode
	}
}

void eva_reset (void)
{

}

uint32_t prev_sys_clock;
void eva_run (uint32_t sys_clock)
{
	if (!GETBIT (eva.s0, 0xF)) listen_startup_magic ();
}
