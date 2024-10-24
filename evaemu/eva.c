#include "eva.h"
#include "evasound.h"
#include "evafx.h"
#include "m68k/m68kcpu.h"

e_byte EVA_RAM[ADDRSPACE 0xFF][ADDRSPACE 0xFFFF];
eva_t eva;

void eva_update_address_bank ( void )
{
	eva.addr_bank = EVA_RAM[0x00][0x0001];
	//printf ( "(EVA) ADDR BANK SET: %x\n", eva.addr_bank );
}

void eva_update_address_port ( void )
{
	eva_update_address_bank ();
	eva.addr = EVA_RAM[0x00][0x0002] << 8 | EVA_RAM[0x00][0x0003];
	//printf ( "(EVA) ADDR PORT SET: %x\n", eva.addr );
}

void eva_update_data_port ( void )
{
	if ( eva.addr_bank == 0 )
	{
		printf ( "(EVA ERROR) UNAUTHORIZED MEM WRITE: BANK %02X ADDR %04X\n", eva.addr_bank, eva.addr );
		return;
	}
	eva.data = EVA_RAM[0x00][0x0004] << 24 | EVA_RAM[0x00][0x0005] << 16 |
			 EVA_RAM[0x00][0x0006] << 8 | EVA_RAM[0x00][0x0007];
	/* transfer from port to actual memory location */
	EVA_RAM[eva.addr_bank][eva.addr] = EVA_RAM[0x00][0x0004];
	EVA_RAM[eva.addr_bank][eva.addr+1] = EVA_RAM[0x00][0x0005];
	EVA_RAM[eva.addr_bank][eva.addr+2] = EVA_RAM[0x00][0x0006];
	EVA_RAM[eva.addr_bank][eva.addr+3] = EVA_RAM[0x00][0x0007];
	printf ( "(EVA) MEM WRITE: BANK %02X ADDR %04X: %08X\n", eva.addr_bank, eva.addr, eva.data );
}

void eva_pulse_reset ( void )
{
	printf ( "(EVA) pulse reset\n" );

	/* clear first RAM bank */
	for ( int i = 0; i <= 0xFFFF; i++ )
	{
		EVA_RAM[0x00][i] = 0x00;
	}
	printf ( "(EVA) interface memory clear\n" );
	/* clear registers */
	eva.data = eva.r0 = eva.r1 = eva.r2 = eva.r3 = 0x00000000;
	eva.flags = eva.addr = 0x0000;
	eva.addr_bank = eva.pc = 0x00;
	eva.soft_boot = false;
	printf ( "(EVA) registers and ports clear\n" );

	/* stop evasound and parse playback */
	for ( int i = 0; i <= 0xFF; i++ )
	{
		StopSound ( evasound.sound_bank[i].bank );
		evasound.sound_bank[i].active = false;
	}
	evasound_parse_sbt ();
	printf ( "(EVA) reset done\n" );
	printf ( "(EVA) CALLING BOOT ROM\n" );
}

void eva_m68k_reset_feedback ( void )
{	
	/* stop evasound and parse playback */
	for ( int i = 0; i <= 0xFF; i++ )
	{
		StopSound ( evasound.sound_bank[i].bank );
		evasound.sound_bank[i].active = false;
	}
}
					/* commands */
void eva_stsp ( void )
{
	PlaySound ( evasound.sound_bank[EVA_RAM[0x00][eva.pc + 3 /* ECT1E LSB */]].bank );
	evasound.sound_bank[EVA_RAM[0x00][eva.pc + 3]].active = true;
}

/*				EVASOUND			*/
void eva_spsp ( void )
{
	StopSound ( evasound.sound_bank[EVA_RAM[0x00][eva.pc + 3 /* ECT1E LSB */]].bank );
	evasound.sound_bank[EVA_RAM[0x00][eva.pc + 3]].active = false;
}

void eva_ssp ( void )
{
	SetSoundPan ( evasound.sound_bank[EVA_RAM[0x00][eva.pc + 3 /* ECT1E LSB */]].bank, ( float ) ( eva.pc + 1 /* ECT0E */ / 255.0f ) );
	printf ("setsoundpan %X\n", eva.pc+1 );
}

/*				EVAFX				*/
void eva_sesp ( void )
{
	e_byte spr_num = EVA_RAM[0x00][eva.pc + 1]; /* ECT0E */
	e_byte spr_w = EVA_RAM[0x00][eva.pc + 4]; /* ECT1 (MSB) */
	e_byte spr_h = EVA_RAM[0x00][eva.pc + 5]; /* ECT1 (MSB+1) */
	e_word spr_vram_index = EVA_RAM[0x00][eva.pc + 2] << 8 | EVA_RAM[0x00][eva.pc + 3];
	evafx.sprite[spr_num].spr = evafx_set_sprite ( EVA_RAM[0x01], spr_w, spr_h, spr_vram_index );
	printf ( "(EVAFX) sprite set: NUM %02X: W %02X H %02X VRAM INDEX %04X\n", spr_num, spr_w, spr_h, spr_vram_index );
}

/*				other				*/
void eva_reset ( void )
{
	eva_pulse_reset ();
	m68k_pulse_reset ();
}

void eva_bootrom_swaprom ( void )
{
	printf ( "(EVA) CALLING GAME ROM\n" );
	eva.soft_boot = true;
	m68k_pulse_reset ();
}
			        	/* ECT process */
void eva_process_ect ( void )
{
	/* if 68K is not writing to ECT, process ECT */
	if ( EVA_RAM[0x00][0xF0] != 0 )
	{
		for ( int i = 0x10; i < 0xF0; i += 16 )
		{
			eva.pc = i;
			switch ( EVA_RAM[0x00][i] )
			{
				default: printf ( "(EVA ERROR) FATAL: %02X: %02X: unrecognized command, ignored\n", 
							eva.pc, i );
				case 0x00: /* nop */ break;
				case 0x01: eva_stsp (); break;
				case 0x02: eva_spsp (); break;
				case 0x03: eva_ssp (); break;
				case 0x0F: eva_sesp (); break;
				case 0xFE: eva_bootrom_swaprom (); break;
				case 0xFF: eva_reset (); break;
			}
		}
		/* clear ECT */
		for ( int i = 0x10; i <= 0xF0; i++ )
		{
			EVA_RAM[0x00][i] = 0x00;
		}
	}
}

void eva_cycle ( void )
{
	eva_process_ect ();
	evasound_handle_loops ();
}

