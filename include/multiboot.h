#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include "types.h"

typedef struct
{
	u16 type;  // Type of the tag
	u16 flags; // Flags for the tag
	u32 size;  // Size of the tag in bytes
} MultibootHeaderTag __attribute__((packed));

typedef struct
{
	u32 magic;
	u32 arch;
	u32 length;
	u32 checksum;
	MultibootHeaderTag tags[];
} MultibootHeader __attribute__((packed));

#endif // !MULTIBOOT_H
