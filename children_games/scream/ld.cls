SECTIONS {
	.text		:	{	avr.o(.vectors)	 avr.o(.text) *(.text) *(.text.*) *(.progmem.data) *(.progmem.*)	}
	. = 0x800000;
	.data		:	{	*(.data)	}
	.bss		:	{
		PROVIDE (__bss_start = .);
		*(.bss)
		PROVIDE (__bss_end = .);
	}
}
