section .text
global load_idt
global isr_syscall

extern syscall_handler

load_idt:
    mov eax, [esp + 4]
    lidt [eax]
    ret

isr_syscall:
    pusha                   ; Save all registers
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10            ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp                ; Pass ESP to syscall handler
    call syscall_handler    ; Call the C handler

    pop gs
    pop fs
    pop es
    pop ds
    popa                    ; Restore registers
    iret                    ; Return to user mode
