pub const MULTIBOOT2_BOOTLOADER_MAGIC = 0x36D76289;
pub const MULTIBOOT2_MAGIC = 0xE85250D6;
pub const MULTIBOOT2_ARCHITECTURE = 0;

// ========================
// Multiboot2 Spec Header
// ========================
pub const Multiboot2Header = extern struct {
    magic: u32,
    architecture: u32,
    header_length: u32,
    checksum: u32,
    //fb_tag: MultibootTagFramebuffer,
    padding1: u32 = 0,
    end_tag: MultibootTag,
};

pub const MultibootTag = extern struct {
    typ: u16,
    flags: u16,
    size: u32,
};

pub const MultibootTagFramebuffer = extern struct {
    tag: MultibootTag,
    width: u32,
    height: u32,
    depth: u8,
};

export var multiboot_header: Multiboot2Header align(8) linksection(".multiboot") = Multiboot2Header{
    .magic = MULTIBOOT2_MAGIC,
    .architecture = MULTIBOOT2_ARCHITECTURE, // i386
    .header_length = @sizeOf(Multiboot2Header),
    .checksum = 0x100000000 - (MULTIBOOT2_MAGIC + MULTIBOOT2_ARCHITECTURE + @sizeOf(Multiboot2Header)),
    // .fb_tag = .{
    //     .tag = .{
    //         .typ = 5,
    //         .flags = 0,
    //         .size = @sizeOf(MultibootTagFramebuffer),
    //     },
    //     .width = 800,
    //     .height = 600,
    //     .depth = 0,
    // },
    .end_tag = .{
        .typ = 0,
        .flags = 0,
        .size = @sizeOf(MultibootTag),
    },
};

pub fn validateBootloaderMagic(magic: u32) bool {
    return magic == MULTIBOOT2_BOOTLOADER_MAGIC;
}

pub fn validateInfoAlignment(mb_info: u32) bool {
    return mb_info & 0x7 == 0;
}
