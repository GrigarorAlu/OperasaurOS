SECTIONS
{
	. = 0x7C00;

	.text :
	{
		*(.boot)
		*(.text)
	}
}
