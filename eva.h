#ifndef EVA_H
#define EVA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "memory.h"

typedef enum
{
	CAX,
	RAX,
	XAX
} mmap_mode_t;

typedef struct
{
	uint32_t r0, r1, r2, r3;
	uint32_t f0, f1, f2, f3;
	uint8_t us;
	uint8_t ss;
	uint8_t cc;
	uint32_t addr;
	uint16_t data;
	mmap_mode_t mmap_mode;
	mem_region_t region_evram;
	mem_region_t region_ewram;
	mem_region_t region_esram;
	mem_region_t region_tram;
} eva_state;
extern eva_state eva;

void eva_reset (void);
void eva_run (uint32_t sys_clock);
void listen_startup_magic (void);
void trigger_command_table (void);


#endif
