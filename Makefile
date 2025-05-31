CC = gcc
LD = ld
ASM = nasm 
QEMU = qemu-system-i386

CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -fno-pie -fno-pic \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c \
         -ffreestanding -std=gnu99 -O2 -Iinclude -g
ASMFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T src/arch/i386/linker.ld -nostdlib

SRC_DIR = src
BUILD_DIR = build
ISO_DIR = iso
ISO_BOOT_DIR = $(ISO_DIR)/boot
ISO_GRUB_DIR = $(ISO_DIR)/boot/grub

C_SOURCES = $(shell find $(SRC_DIR) -name "*.c")
ASM_SOURCES = $(shell find $(SRC_DIR) -name "*.asm")
HEADERS = $(shell find $(SRC_DIR) -name "*.h")

C_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
ASM_OBJECTS = $(patsubst $(SRC_DIR)/arch/i386/%.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

KERNEL = $(BUILD_DIR)/kernel.elf
ISO = $(BUILD_DIR)/wolfieOS.iso

all: $(ISO)

$(BUILD_DIR)/%.o: $(SRC_DIR)/arch/i386/%.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

$(KERNEL): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

$(ISO_GRUB_DIR):
	mkdir -p $(ISO_GRUB_DIR)

$(ISO_GRUB_DIR)/grub.cfg: | $(ISO_GRUB_DIR)
	echo 'menuentry "WolfieOS" {' >> $@
	echo '    multiboot2 /boot/kernel.elf' >> $@
	echo '    boot' >> $@
	echo '}' >> $@

$(ISO_BOOT_DIR)/kernel.elf: $(KERNEL) | $(ISO_GRUB_DIR)
	cp $< $@

$(ISO): $(ISO_BOOT_DIR)/kernel.elf $(ISO_GRUB_DIR)/grub.cfg
	grub-mkrescue -o $@ $(ISO_DIR)

run: $(ISO)
	$(QEMU) -cdrom $(ISO) -m 512M

debug: $(ISO)
	$(QEMU) -cdrom $(ISO) -m 512M -s -S &
	gdb -ex "target remote localhost:1234" -ex "symbol-file $(KERNEL)"

clean:
	rm -rf $(BUILD_DIR)/**/*.o $(ISO_DIR)

.PHONY: all clean run debug
