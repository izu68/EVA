// -------------------------------------------------------------------------------
// EVA - Firmware main file
// Contents :
//		Memory addressing handlers
//		System state handlers
//		ECT runtime
//		Main system cycle
// Notes :
//		*
// -------------------------------------------------------------------------------

#include "eva.h"
#include "evasound.h"
#include "evafx.h"
//#include "../../maria/backend/m68k/m68kcpu.h"

e_byte EVA_CONTROL[ADDRSPACE 0xFF];
e_byte EVA_VRAM[ADDRSPACE 0xFFFF];
e_byte EVA_TRAM[ADDRSPACE 0xFFFF];
e_byte EVA_WRAM[ADDRSPACE 0x17FFF];
eva_t eva;

// =============================== HANDLERS ======================================

/*
		e_byte type -> 0 = byte, 1 = word, 2 = long
*/
void eva_update_address_port ( e_byte type )
{
	if ( type == 1 )
	{
		eva.addr = EVA_CONTROL[0x02] << 8 | EVA_CONTROL[0x03];
	}
	else if ( type == 2 )
	{
		eva.addr = EVA_CONTROL[0x00] << 24 | EVA_CONTROL[0x01] << 16 | EVA_CONTROL[0x02] << 8 | EVA_CONTROL[0x03];
	}
}

void eva_update_data_port ( e_byte type )
{
	if ( type == 0 )
	{
		EVA_WRAM[eva.addr] = EVA_CONTROL[0x07];
	}
	else if ( type == 1 )
	{
		EVA_WRAM[eva.addr] = EVA_CONTROL[0x06];
		EVA_WRAM[eva.addr+1] = EVA_CONTROL[0x07];
	}
	else if ( type == 2 )
	{
		EVA_WRAM[eva.addr] = EVA_CONTROL[0x04];
		EVA_WRAM[eva.addr+1] = EVA_CONTROL[0x05];
		EVA_WRAM[eva.addr+2] = EVA_CONTROL[0x06];
		EVA_WRAM[eva.addr+3] = EVA_CONTROL[0x07];
	}
}

void eva_update_registers ( void )
{
	/*
	// Update internal registers with data from control region mirror
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
	*/
}

void eva_pulse_reset ( void )
{
	printf ( "(EVA) Pulse reset\n" );

	// Clear control region
	for ( int i = 0; i <= 0xFF; i++ )
	{
		EVA_CONTROL[i] = 0x00;
	}
	printf ( "(EVA) Control region cleared\n" );

	// Clear internal registers
	eva.addr = eva.data =
	eva.r0 = eva.r1 = eva.r2 = eva.r3 =
	eva.f0 = eva.f1 = eva.f2 = eva.f3 =
	eva.flags = eva.pc = 0;
	// Set HRC to BANK 2 and specify hard boot state
	eva.soft_boot = false;

	printf ( "(EVA) Registers cleared\n" );

	// Stop all sounds and parse SBT
	for ( int i = 0; i <= 0xFF; i++ )
	{
		//StopSound ( evasound.sound_bank[i].bank );
		evasound.sound_bank[i].active = false;
	}

	evasound_parse_sbt ();
	//printf ( "(EVA) Pass control to BIOS\n" );
}

void eva_m68k_reset_feedback ( void )
{	
	// Clear internal registers
	eva.addr = eva.data =
	eva.r0 = eva.r1 = eva.r2 = eva.r3 =
	eva.f0 = eva.f1 = eva.f2 = eva.f3 =
	eva.flags = eva.pc = 0;
	// Stop all sounds
	for ( int i = 0; i <= 0xFF; i++ )
	{
		//StopSound ( evasound.sound_bank[i].bank );
		evasound.sound_bank[i].active = false;
	}
}

// =============================== INSTRUCTIONS ==================================

void eva_reset ( void )
{
	// Called from ECT (BIOS)
	eva_pulse_reset ();
	//m68k_pulse_reset ();
}

void eva_bios_swaprom ( void )
{
	// Called from ECT (BIOS)
	printf ( "(EVA) BIOS EXEC GAME\n" );
	eva.soft_boot = true;
	//m68k_pulse_reset (); // Will also pass feedback to EVA
}

// =============================== EXECUTION =====================================

void eva_process_ect ( void )
{
	// If 68K is not writing to ECT, process ECT
	if ( EVA_CONTROL[0xF0] != 0 )
	{
		for ( int i = 0x70; i < 0xF0; i += 8 )
		{
			eva.pc = i;
			switch ( EVA_CONTROL[i] )
			{
				default: printf ( "(EVA ERROR) FATAL: %02X: %02X: unrecognized command, ignored\n", 
						  eva.pc, i );
				case 0x00: /* nop */ break;
				case 0x01: eva_psnd (); break;
				case 0x02: eva_ssnd (); break;
				case 0x03: eva_sspa (); break;
				case 0x10: eva_uptram (); break;
				case 0x11: eva_ltsp (); break;
				case 0xEF: eva_reset (); break;
				case 0xF0: eva_bios_swaprom (); break;
			}
		}
		// Clear ECT (00F0 (ECT lock) also set to 0)
		for ( int i = 0x70; i <= 0xF0; i++ )
		{
			EVA_CONTROL[i] = 0x00;
		}
	}
}

void eva_cycle ( void )
{
	// Firmware loop
	eva_process_ect ();
	evasound_handle_loops ();
}

