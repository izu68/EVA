#ifndef PCM_H
#define PCM_H

#include "eva.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

#include "external/raudio.h"

typedef struct
{
	Sound bank;
	bool active;
	bool loop;
} pcm_bank_t;

void load_pcm_banks (void);
void evasound_handle_loops (void);

void eva_psnd ( void );
void eva_ssnd ( void );

#endif
