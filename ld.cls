SECTIONS {
	. = 0x0;
	.text		:	{	*(.text)	}
	. = 0x800000;
	.data		:	{	*(.data)	}
	.bss		:	{	*(.bss)		}
}
