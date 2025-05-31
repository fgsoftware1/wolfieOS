#include "types.h"
#include "kernel.h"

const unsigned int MULTIBOOT_BOOTLOADER_MAGIC = 0x36d76289;
const unsigned int MULTIBOOT_MAGIC = 0xE85250D6;
const unsigned int MULTIBOOT_ARCH_I386 = 0;

u8 stack_bottom[4096 * 4] __attribute__((section(".bss"), aligned(16)));
u8* stack_top = (u8*)((uintptr_t)&stack_bottom)+ sizeof(typeof(stack_bottom));

MultibootHeader multiboot_header __attribute__((used, section(".multiboot"), aligned(8))) = {
	.magic = MULTIBOOT_MAGIC,
	.arch = MULTIBOOT_ARCH_I386,
	.length = sizeof(MultibootHeader),
	.checksum = -(MULTIBOOT_MAGIC + MULTIBOOT_ARCH_I386 + sizeof(MultibootHeader)),
	.tags = {
		{.type = 0, .flags = 0, .size = sizeof(MultibootHeaderTag)}
	}
};

__attribute__((naked, noreturn)) void _start()
{
	asm volatile(
		"cli\n"
		"xor %%ebp, %%ebp\n"
		"movl %0, %%esp\n"
		"movl %%esp, %%ebp\n"
		"push %%ebx\n"
		"push %%eax\n"
		"call *%1\n"
		"hlt"
		:
		: "r"(&stack_top),
		  "r"(&kmain)
		: "memory", "ebx", "eax"
	);
	__builtin_unreachable();
}

void kmain(u32 magic, u32 multiboot_info)
{
#if defined(__i386__)
	kmain32(magic, multiboot_info);
#elif defined(__x86_64__)
#error "64-bit not supported yet"
#else
#error "Unsupported architecture"
#endif
}
