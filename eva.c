#include "eva.h"
#include "pcm.h"
#include <time.h>

eva_state eva;

#define CYCLE_TIME_NS 2083 // Simulate ~480 MHz clock (2.083 ns per cycle)

// Note
// This is here for when I compensate the speed difference between the host
// computer running this logic and the STM32H7. The function makes a very rough
// approximation of the runtime speed of a 480 MHz CPU.
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

void init_eva (void)
{
	printf ("(EVA) System startup requested\n");

	// Stop potential channels that were already playing
	halt_channel (-1);

	// Set memory map mode
	uint8_t mmap_mode = CONTROL[0x12];
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
		case CAX: map_region (&eva.region_evram, 0x3D0000, 0x3D7FFF);
			  map_region (&eva.region_esram, 0x3D8000, 0x3D8FFF); 
			  map_region (&eva.region_ewram, 0x3E0000, 0x3FFFFF);
			  break;
		case RAX: map_region (&eva.region_evram, 0x400000, 0x407FFF);
			  map_region (&eva.region_esram, 0x408000, 0x408FFF);
			  map_region (&eva.region_ewram, 0x410000, 0x42FFFF);
			  break;
		case XAX: map_region (&eva.region_evram, 0x7D0000, 0x7D7FFF);
			  map_region (&eva.region_esram, 0x7D8000, 0x7D8FFF);
			  map_region (&eva.region_ewram, 0x7E0000, 0x7FFFFF);
			  break;
	}		

	// Clear registers
	eva.r0 = eva.r1 = eva.r2 = eva.r3 = 0;
	eva.f0 = eva.f1 = eva.f2 = eva.f3 = 0;
	eva.cc = 0;
	eva.us = 0;
	eva.ss = 0;
	eva.addr = 0;
	eva.data = 0;
	printf ("(EVA) Registers initialized\n");

	// System Status register initialization
	SETBIT (eva.ss, 0x7); // System active

	init_pcm_system ();
}

void listen_startup_magic (void)
{
	if (CONTROL[0x13] == 'E' && CONTROL[0x14] == 'V' && CONTROL[0x15] == 'A')
	{
		init_eva ();
	}
}

void reset_eva (void)
{

}

void trigger_command_table (void)
{
	SETBIT (eva.ss, 0x6); // ECT Runtime flag
	for (uint8_t i = 0x20; i < 0xB0; i += 8)
	{
		eva.cc = i;
		switch (CONTROL[i])
		{
			default: /* illegal command */ break;
			case 0x00: goto _break_run; break;
			case 0x01: load_sound_bank (CONTROL[i + 1]); break;
			case 0x02: play_sound_bank (CONTROL[i + 1], CONTROL[i + 2]); break;
			case 0x03: halt_channel (CONTROL[i + 1]); break;
		}
	}
	_break_run:
	for (uint8_t i = 0x20; i <= 0xB0; i++)
	{
		CONTROL[i] = 0x00;
	}
	CLRBIT (eva.ss, 0x6);
	eva.cc = 0;
}

void eva_run (uint32_t sys_clock)
{
	if (!GETBIT (eva.ss, 0x7)) 
	{
		listen_startup_magic ();
	}
}
