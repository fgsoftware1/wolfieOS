ENTRY(_start)
OUTPUT_FORMAT("elf32-i386")

SECTIONS
{
    . = 2M;

    .multiboot ALIGN(4K) : {
        KEEP(*(.multiboot))
    }

    .text ALIGN(4K) : {
    *(.text*)
  }

  .rodata ALIGN(4K) : {
    *(.rodata*)
  }

  .data ALIGN(4K) : {
    *(.data*)
  }

  .bss ALIGN(4K) : {
    *(COMMON)
    *(.bss*)
  }

  /DISCARD/ : {
    *(.comment)
    *(.note*)
	*(.eh_frame)
  }
}
