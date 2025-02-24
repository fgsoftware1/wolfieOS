bits 32

section .multiboot
align 8
header_start:
	dd 0xe85250d6 ; multiboot2 magic number
	dd 0 ; protected mode i386
	dd header_end - header_start ;hedaer size
	dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) ; checksum

	dw 0
	dw 0
	dd 8
header_end:

section .bss
align 16

global stack_top
stack_bottom:
    resb 16384 ;16kib
stack_top:

section .text
    global _start

_start:
    mov esp, stack_top
    extern kmain
    push ebx
    call kmain
loop:
    jmp loop
