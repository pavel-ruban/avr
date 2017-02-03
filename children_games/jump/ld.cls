SECTIONS {
	.text		:	{	avr.o(.vectors)	 avr.o(.text) *(.text) *(.text.*) *(.progmem.data)	}
	. = 0x800000;
	.data		:	{	*(.data)	}
	.bss		:	{	*(.bss)		}
}
