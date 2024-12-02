#include "pcm.h"

pcm_bank_t pcm_bank[ADDRSPACE 0xFF];

void dac1 (uint8_t ch1_sample)
{

}

void dac2 (uint8_t ch1_sample, uint8_t ch2_sample)
{

}



void load_pcm_banks ( void )
{

	FILE *file;
	int sid;
	int loop;
	char ver_str[4];
	char path_str[64];
	
	file = fopen ( "flash/pcm", "r" );
	if ( file == NULL )
	{
		printf ( "(EVA ERROR) PCM bank table not found\n" );
		return;
	}

	// Parse table: Get Sound ID, Sound path, Loop flag
    	while ( fscanf ( file, "%x %s %d", &sid, path_str, &loop ) == 3 )
	{
		if ( sid > 0xFF )
		{
			printf ( "(EVA ERROR) Exceded maximum sound bank allocation\n" );
			fclose ( file );
			return;
		}
		pcm_bank[sid].bank = LoadSound ( path_str );
		pcm_bank[sid].loop = loop;
		if ( pcm_bank[sid].bank.frameCount < 1 )
		{
			printf ( "(EVA ERROR) Failed to allocate sound 0x%x\n", sid );
			fclose ( file );
			return;
		}
		printf ( "(EVA) allocated sound bank %02X\n", sid );
    	}

    	fclose ( file );
}

void evasound_handle_loops ( void )
{
	// Check if sound should be playing, is not playing and loop flag to loop
	for ( int i = 0; i <= 0xFF; i++ )
	{
		if 
		(
			pcm_bank[i].active && pcm_bank[i].loop && 
			!IsSoundPlaying ( pcm_bank[i].bank ) 
		)
		{
			PlaySound ( pcm_bank[i].bank );
		}
	}
}



void eva_psnd ( void )
{
	// Called from ECT
	PlaySound ( pcm_bank[CONTROL[eva.cc + 1 /* ECT1E LSB */]].bank );
	pcm_bank[CONTROL[eva.cc + 1]].active = true;
}

void eva_ssnd ( void )
{
	// Called from ECT
	StopSound ( pcm_bank[CONTROL[eva.cc + 1 /* ECT1E LSB */]].bank );
	pcm_bank[CONTROL[eva.cc + 1]].active = false;
}

