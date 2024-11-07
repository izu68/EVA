// -------------------------------------------------------------------------------
// EVA - Firmware sound module
// Contents :
//		Sound Bank Table parsing
//		Sound playback handlers
//		Sound instructions
// Notes :
//		*
// -------------------------------------------------------------------------------

#include "evasound.h"

evasound_t evasound;

// =============================== HANDLERS ======================================

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

	// Parse SBT: Get Sound ID, Sound path, Loop flag
    	while ( fscanf ( file, "%x %s %d", &sid, path_str, &loop ) == 3 )
	{
		if ( sid > 0xFF )
		{
			printf ( "(EVA ERROR) Exceded maximum sound bank allocation\n" );
			fclose ( file );
			return
		}
		evasound.sound_bank[sid].bank = LoadSound ( path_str );
		evasound.sound_bank[sid].loop = loop;
		if ( evasound.sound_bank[sid].bank.frameCount < 1 )
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
		if ( evasound.sound_bank[i].active && evasound.sound_bank[i].loop && !IsSoundPlaying ( evasound.sound_bank[i].bank ) )
		{
			PlaySound ( evasound.sound_bank[i].bank );
		}
	}
}


// =============================== INSTRUCTIONS ==================================

void eva_psnd ( void )
{
	// Called from ECT
	PlaySound ( evasound.sound_bank[EVA_RAM[0x00][eva.pc + 3 /* ECT1E LSB */]].bank );
	evasound.sound_bank[EVA_RAM[0x00][eva.pc + 1]].active = true;
}

void eva_ssnd ( void )
{
	// Called from ECT
	StopSound ( evasound.sound_bank[EVA_RAM[0x00][eva.pc + 3 /* ECT1E LSB */]].bank );
	evasound.sound_bank[EVA_RAM[0x00][eva.pc + 1]].active = false;
}

void eva_sspa ( void )
{
	// Called from ECT
	SetSoundPan ( evasound.sound_bank[EVA_RAM[0x00][eva.pc + 3 /* ECT1E LSB */]].bank, ( float ) ( eva.pc + 1 /* ECT0E */ / 255.0f ) );
	printf ("setsoundpan %X\n", eva.pc+1 );
}
