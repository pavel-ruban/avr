SECTIONS {
	.text	0x0		:	{	*(.text)	}
	.data	0x800000	:	{	*(.data)	}
	.bss			:	{	*(.bss)		}
}
