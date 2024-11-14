int main ( void )
{
	return 0;
}

__attribute__ ( ( naked, noreturn ) ) void _reset ( void )
{
	// Memset .bss to zero, then copy .data to RAM
	extern long _sbss, _ebss, _sdata, _edata, _sidata;
  	for ( long *dst = &_sbss; dst < &_ebss; dst++ ) *dst = 0;
  	for ( long *dst = &_sdata, *src = &_sidata; dst < &_edata; ) *dst++ = *src++;

	main ();
	for ( ;; ) ( void ) 0;
}

// 16 standard and 91 STM32-specific handlers
__attribute__ ( ( section ( ".vectors" ) ) ) 
void ( *const tab[16 + 91])( void ) = { ( void* ) 0x3001FFFF, _reset }; // Stack pointer (high SRAM)
