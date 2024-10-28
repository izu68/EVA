#ifndef H_EVASOUND
#define H_EVASOUND

#include "eva.h"
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

#define ADDRSPACE 1+

typedef struct
{
	Sound bank;
	bool active;
	bool loop;
} sound_bank_t;

typedef struct
{
	sound_bank_t sound_bank[ADDRSPACE 0xFF];
} evasound_t;
extern evasound_t evasound;

void evasound_parse_sbt ( void );
void evasound_handle_loops ( void );

void eva_stsp ( void );
void eva_spsp ( void );
void eva_ssp ( void );

#endif
