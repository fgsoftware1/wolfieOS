# Compiler and linker settings
CC = gcc
LD = ld
ASM = as
QEMU = qemu-system-i386

# Compiler flags
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -fno-pie -fno-pic \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c \
         -ffreestanding -std=gnu99 -O2 -Isrc -g

# Assembler flags
ASMFLAGS = --32

# Linker flags - note we're not stripping debug symbols
LDFLAGS = -m elf_i386 -T src/linker.ld -nostdlib

# Directories
SRC_DIR = src
BUILD_DIR = build
ISO_DIR = iso
ISO_BOOT_DIR = $(ISO_DIR)/boot
ISO_GRUB_DIR = $(ISO_DIR)/boot/grub

# Find all source files recursively
C_SOURCES = $(shell find $(SRC_DIR) -name "*.c")
ASM_SOURCES = $(shell find $(SRC_DIR) -name "*.s")
HEADERS = $(shell find $(SRC_DIR) -name "*.h")

# Generate object file paths
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
ASM_OBJECTS = $(patsubst $(SRC_DIR)/%.s, $(BUILD_DIR)/%.o, $(ASM_SOURCES))
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Output files - now using .elf extension
KERNEL = $(BUILD_DIR)/kernel.elf
ISO = $(BUILD_DIR)/wolfieOS.iso

# Default target
all: $(ISO)

# Create build directory structure for object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

# Link object files into an ELF executable
$(KERNEL): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

# Create ISO directories
$(ISO_GRUB_DIR):
	mkdir -p $(ISO_GRUB_DIR)

# Create GRUB configuration
$(ISO_GRUB_DIR)/grub.cfg: | $(ISO_GRUB_DIR)
	echo 'menuentry "WolfieOS" {' >> $@
	echo '    multiboot2 /boot/kernel.elf' >> $@
	echo '    boot' >> $@
	echo '}' >> $@

# Copy kernel to ISO directory
$(ISO_BOOT_DIR)/kernel.elf: $(KERNEL) | $(ISO_GRUB_DIR)
	cp $< $@

# Create ISO image
$(ISO): $(ISO_BOOT_DIR)/kernel.elf $(ISO_GRUB_DIR)/grub.cfg
	grub-mkrescue -o $@ $(ISO_DIR)

# Run QEMU
run: $(ISO)
	$(QEMU) -cdrom $(ISO) -m 512M

# Debug with QEMU and GDB
debug: $(ISO)
	$(QEMU) -cdrom $(ISO) -m 512M -s -S &
	gdb -ex "target remote localhost:1234" -ex "symbol-file $(KERNEL)"

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)

# Phony targets
.PHONY: all clean run debug