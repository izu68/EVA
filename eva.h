#ifndef H_EVA
#define H_EVA

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define e_byte 	unsigned char
#define e_word 	unsigned short
#define e_long 	unsigned int
#define e_flt	float

#define ADDRSPACE 1+
extern e_byte EVA_RAM[ADDRSPACE 0xF][ADDRSPACE 0xFFFF]; // 1MB of memory
/*
		   EVA MEMORY MAP
	BANK 00: 0001 (A13001): ADDR BANK
	BANK 00: 0002 (A13002): ADDR PORT
	BANK 00: 0004-0007 (A13004-A13007): DATA PORT
	
	BANK 00: 0010-001F (A13010-A1301F): R0-R3
	BANK 00: 0020-002F (A13020-A1302F): F0-F3

	BANK 00: 0070-00EF (A13070-A130EF): ECT

	BANK 01: 0000-FFFF (3E0000-3EFFFF): HRC MIRROR
	BANK 02: 0000-FFFF (3F0000-3FFFFF): VRAM MIRROR
*/

typedef struct
{
	e_long r0, r1, r2, r3;
	e_flt  f0, f1, f2, f3;
	e_byte hrc;	
	e_word flags;
	e_byte pc;

	e_byte addr_bank;
	e_word addr;
	e_long data;

	bool soft_boot;
} eva_t;
extern eva_t eva;

void eva_update_address_bank ( void );
void eva_update_address_port ( void );
void eva_update_data_port ( void );
void eva_update_registers ( void );
void eva_pulse_reset ( void );
void eva_m68k_reset_feedback ( void );
void eva_cycle ( void );

#endif
