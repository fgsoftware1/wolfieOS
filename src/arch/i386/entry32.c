#include "kernel.h"

void kmain32(u32 magic, u32 multiboot_info)
{
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		return;
	}

	if (multiboot_info & 7)
	{
		return;
	}

	while (true)
	{
		asm volatile("hlt");
	}
}
