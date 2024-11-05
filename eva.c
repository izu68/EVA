#include "eva.h"
#include "evasound.h"
#include "evafx.h"
#include "../maria/backend/m68k/m68kcpu.h"

e_byte EVA_RAM[ADDRSPACE 0xFF][ADDRSPACE 0xFFFF];
eva_t eva;

// =============================== HANDLERS ======================================

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
		printf ( "(EVA ERROR) Unauthorized access: BANK %02Xn", eva.addr_bank );
		return;
	}
	eva.data = EVA_RAM[0x00][0x0004] << 24 | EVA_RAM[0x00][0x0005] << 16 |
		   EVA_RAM[0x00][0x0006] << 8  | EVA_RAM[0x00][0x0007];
	/* transfer from port to actual memory location */
	EVA_RAM[eva.addr_bank][eva.addr] = EVA_RAM[0x00][0x0004];
	EVA_RAM[eva.addr_bank][eva.addr+1] = EVA_RAM[0x00][0x0005];
	EVA_RAM[eva.addr_bank][eva.addr+2] = EVA_RAM[0x00][0x0006];
	EVA_RAM[eva.addr_bank][eva.addr+3] = EVA_RAM[0x00][0x0007];
	printf ( "(EVA) MEM Write: BANK %02X ADDR %04X: %08X\n", eva.addr_bank, eva.addr, eva.data );
}

void eva_update_registers ( void )
{
	eva.r0 = EVA_RAM[0x00][0x0010] << 24 | EVA_RAM[0x00][0x0011] << 16 |
		 EVA_RAM[0x00][0x0012] << 8  | EVA_RAM[0x00][0x0013];
	eva.r1 = EVA_RAM[0x00][0x0014] << 24 | EVA_RAM[0x00][0x0015] << 16 |
		 EVA_RAM[0x00][0x0016] << 8  | EVA_RAM[0x00][0x0017];
	eva.r2 = EVA_RAM[0x00][0x0018] << 24 | EVA_RAM[0x00][0x0019] << 16 |
		 EVA_RAM[0x00][0x001A] << 8  | EVA_RAM[0x00][0x001B];
	eva.r3 = EVA_RAM[0x00][0x001C] << 24 | EVA_RAM[0x00][0x001D] << 16 |
		 EVA_RAM[0x00][0x001E] << 8  | EVA_RAM[0x00][0x001F];

	eva.f0 = EVA_RAM[0x00][0x0020] << 24 | EVA_RAM[0x00][0x0021] << 16 |
		 EVA_RAM[0x00][0x0022] << 8  | EVA_RAM[0x00][0x0023];
	eva.f1 = EVA_RAM[0x00][0x0024] << 24 | EVA_RAM[0x00][0x0025] << 16 |
		 EVA_RAM[0x00][0x0026] << 8  | EVA_RAM[0x00][0x0027];
	eva.f2 = EVA_RAM[0x00][0x0028] << 24 | EVA_RAM[0x00][0x0029] << 16 |
		 EVA_RAM[0x00][0x002A] << 8  | EVA_RAM[0x00][0x002B];
	eva.f3 = EVA_RAM[0x00][0x002C] << 24 | EVA_RAM[0x00][0x002D] << 16 |
		 EVA_RAM[0x00][0x002E] << 8  | EVA_RAM[0x00][0x002F];
}

void eva_pulse_reset ( void )
{
	/* clear first RAM bank */
	for ( int i = 0; i <= 0xFFFF; i++ )
	{
		EVA_RAM[0x00][i] = 0x00;
	}
	printf ( "(EVA) Interface memory clear\n" );

	/* clear registers */
	eva.addr_bank = eva.addr = eva.data =
	eva.r0 = eva.r1 = eva.r2 = eva.r3 =
	eva.f0 = eva.f1 = eva.f2 = eva.f3 =
	eva.flags = eva.pc = 0;
	eva.hrc = 2;
	eva.soft_boot = false;
	printf ( "(EVA) Registers and ports clear\n" );

	/* stop evasound and parse playback */
	for ( int i = 0; i <= 0xFF; i++ )
	{
		StopSound ( evasound.sound_bank[i].bank );
		evasound.sound_bank[i].active = false;
	}
	evasound_parse_sbt ();
	printf ( "(EVA) Pulse reset\n" );
	printf ( "(EVA) Hardware jump: BIOS\n" );
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

// =============================== INSTRUCTIONS ==================================

void eva_reset ( void )
{
	eva_pulse_reset ();
	m68k_pulse_reset ();
}

void eva_bios_swaprom ( void )
{
	printf ( "(EVA) BIOS jump: GAME\n" );
	eva.soft_boot = true;
	m68k_pulse_reset ();
}

// =============================== EXECUTION =====================================

void eva_process_ect ( void )
{
	/* if 68K is not writing to ECT, process ECT */
	if ( EVA_RAM[0x00][0xF0] != 0 )
	{
		for ( int i = 0x70; i < 0xF0; i += 8 )
		{
			eva.pc = i;
			switch ( EVA_RAM[0x00][i] )
			{
				default: printf ( "(EVA ERROR) FATAL: %02X: %02X: unrecognized command, ignored\n", 
						  eva.pc, i );
				case 0x00: /* nop */ break;
				case 0x01: eva_psnd (); break;
				case 0x02: eva_ssnd (); break;
				case 0x03: eva_sspa (); break;
				case 0x08: eva_pps (); break;
				case 0x09: eva_rgfx (); break;
				case 0xEF: eva_reset (); break;
				case 0xF0: eva_bios_swaprom (); break;
			}
		}
		/* clear ECT */
		for ( int i = 0x70; i <= 0xF0; i++ )
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

