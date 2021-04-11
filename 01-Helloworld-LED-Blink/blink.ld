MEMORY
{
	flash 	(rx)	: ORIGIN = 0x8000000, LENGTH = 64K
	sram 	(!rx)	: ORIGIN = 0x20000000, LENGTH = 20K
}

/* Define output sections */
SECTIONS
{
	.vectors	:{*(.vectors)}>flash
	.text	:{*(.text)}>flash
	.rodata	:{*(.rodata)}>flash
	.bss	:{*(.bss)}>sram
}