ENTRY(_reset);
MEMORY 
{
  	flash(rx)  	: ORIGIN = 0x08000000, LENGTH = 2048k
	control(rw)	: ORIGIN = 0x30000000, LENGTH = 0xFF
  	firmram(rwx)  	: ORIGIN = 0x300000FF, LENGTH = 0x1FF00
}

SECTIONS 
{
	.vectors  : { KEEP(*(.vectors)) }   > flash
  	.text     : { *(.text*) }           > flash
  	.rodata   : { *(.rodata*) }         > flash

  	.data : 
	{
    		_sdata = .;   /* .data section start */
    		*(.first_data)
    		*(.data SORT(.data.*))
    		_edata = .;  /* .data section end */
  	} > firmram AT > flash
  	_sidata = LOADADDR(.data);

  	.bss : 
	{
    		_sbss = .;              /* .bss section start */
    		*(.bss SORT(.bss.*) COMMON)
    		_ebss = .;              /* .bss section end */
  	} > firmram

  	. = ALIGN(8);
  	_end = .;     /* for cmsis_gcc.h  */
}
