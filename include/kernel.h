#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"
#include "multiboot.h"

extern const unsigned int MULTIBOOT_BOOTLOADER_MAGIC;
extern const unsigned int MULTIBOOT_MAGIC;
extern const unsigned int MULTIBOOT_ARCH_I386;

extern u8 stack_bottom[4096 * 4];
extern u8 *stack_top;

void kmain(u32 magic, u32 multiboot_info);
#if defined(__i386__)
void kmain32(u32 magic, u32 multiboot_info);
#elif defined(__x86_64__)
void kmain64(u32 magic, u32 multiboot_info);
#endif

#endif // !KERNEL_H
