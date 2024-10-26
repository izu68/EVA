#include "evasound.h"

evasound_t evasound;

void evasound_parse_sbt ( void )
{
	FILE *file;
	int sid;
	int loop;
	char ver_str[4];
	char path_str[64];
	
	file = fopen ( "flash/sbt", "r" );
	if ( file == NULL )
	{
		printf ( "(EVA ERROR) sound bank table not found\n" );
		return;
	}

	/* parse cue file */
    	while ( fscanf ( file, "%x %s %d", &sid, path_str, &loop ) == 3 )
	{
		if ( sid > 0xFF )
		{
			printf ( "(EVA) exceded maximum bank allocation\n" );
		}
		evasound.sound_bank[sid].bank = LoadSound ( path_str );
		evasound.sound_bank[sid].loop = loop;
		if ( evasound.sound_bank[sid].bank.frameCount < 1 )
		{
			printf ( "(EVA ERROR) failed to allocate sound 0x%x\n", sid );
			fclose ( file );
			return;
		} 
		printf ( "(EVA) allocated sound bank %02X\n", sid );
    	}

    	fclose ( file );
}

void evasound_handle_loops ( void )
{
	/* check if a sound has the loop flag and if it needs to loop */
	for ( int i = 0; i <= 0xFF; i++ )
	{
		if ( evasound.sound_bank[i].active && evasound.sound_bank[i].loop && !IsSoundPlaying ( evasound.sound_bank[i].bank ) )
		{
			PlaySound ( evasound.sound_bank[i].bank );
		}
	}
}
