ENTRY(_start)

SECTIONS
{
    . = 2M;

    .multiboot : {
        *(.multiboot)
    }

    .text 0x0100000 : {
        *(.text)
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .rodata : {
        *(.rodata)
    }

    .bss : {
        *(.bss)
    }
}
