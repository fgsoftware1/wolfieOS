const io = @import("../../utils/io.zig");
const boot = @import("../../boot.zig");

pub const GDT_SELECTOR = struct {
    pub const NULL: u16        = 0x00;
    pub const KERNEL_CODE: u16 = 0x08;
    pub const KERNEL_DATA: u16 = 0x10;
    pub const USER_CODE: u16   = 0x18;
    pub const USER_DATA: u16   = 0x20;
    pub const TSS: u16         = 0x28;
};

pub const GDTPtr = packed struct {
    size: u16, // Size of GDT - 1
    addr: u32,  // Physical address of GDT
};

const GDTEntry = packed struct {
    limit_low: u16,
    base_low: u16,
    base_middle: u8,
    access: u8,
    granularity: u8,
    base_high: u8,
};

const TSS = packed struct {
    prev_tss: u16,
    reserved1: u16,
    esp0: u32,
    ss0: u16,
    reserved2: u16,
    esp1: u32,
    ss1: u16,
    reserved3: u16,
    esp2: u32,
    ss2: u16,
    reserved4: u16,
    cr3: u32,
    eip: u32,
    eflags: u32,
    eax: u32,
    ecx: u32,
    edx: u32,
    ebx: u32,
    esp: u32,
    ebp: u32,
    esi: u32,
    edi: u32,
    es: u16,
    reserved5: u16,
    cs: u16,
    reserved6: u16,
    ss: u16,
    reserved7: u16,
    ds: u16,
    reserved8: u16,
    fs: u16,
    reserved9: u16,
    gs: u16,
    reserved10: u16,
    ldtr: u16,
    reserved11: u16,
    trap: u16,
    io_permissions_base_offset: u16,
};

var gdt = [_]u64{
    // Null descriptor
    0x0000000000000000,
    // Kernel code (0x08)
    createDescriptor(0, 0xFFFFF, 0x9A, 0xCF),
    // Kernel data (0x10)
    createDescriptor(0, 0xFFFFF, 0x92, 0xCF),
    // User code (0x18)
    createDescriptor(0, 0xFFFFF, 0xFA, 0xCF),
    // User data (0x20)
    createDescriptor(0, 0xFFFFF, 0xF2, 0xCF),
    // TSS descriptor (0x28)
    0,
};

var gdtr: GDTPtr = undefined;
var tss: TSS    = undefined;

// zig fmt: off
pub fn load() void {
    gdtr = .{
        .limit = @sizeOf(@TypeOf(gdt)) - 1,
        .base  = @intFromPtr(&gdt),
    };

    io.lgdt(&gdtr);

    // Reload CS with far jump
    asm volatile (
        \\ ljmp $0x08, $flush
        \\ flush:
        :
        :
        : "memory"
    );

    // Reload data segments
    asm volatile (
        \\ mov $0x10, %%ax
        \\ mov %%ax, %%ds
        \\ mov %%ax, %%es
        \\ mov %%ax, %%fs
        \\ mov %%ax, %%gs
        \\ mov %%ax, %%ss
        :
        :
        : "ax", "memory"
    );

    init_tss();
}

fn createDescriptor(base: u32, limit: u32, access: u8, gran: u8) u64 {
    const limit_low   = limit & 0xFFFF;        // bits 0..15
    const base_low    = base  & 0xFFFF;        // bits 16..31
    const base_mid    = (base >> 16) & 0xFF;   // bits 32..39
    const access_byte = access;                // bits 40..47
    const limit_high  = (limit >> 16) & 0x0F;  // bits 48..51
    const flags       = gran & 0x0F;           // bits 52..55
    const base_high   = (base >> 24) & 0xFF;   // bits 56..63

    return  (@as(u64, limit_low)        << 0)
         | (@as(u64, base_low)         << 16)
         | (@as(u64, base_mid)         << 32)
         | (@as(u64, access_byte)      << 40)
         | (@as(u64, limit_high)       << 48)
         | (@as(u64, flags)            << 52)
         | (@as(u64, base_high)        << 56);
}
// zig fmt: on

fn init_tss() void {
    const tss_selector: u16 = GDT_SELECTOR.TSS;
    const tss_base          = @intFromPtr(&tss);
    const tss_limit         = @sizeOf(TSS) - 1;
    const stack_top         = @intFromPtr(&boot.stack_bytes) + @sizeOf(@TypeOf(boot.stack_bytes));

    tss.esp0 = @as(u32, stack_top);
    tss.ss0  = GDT_SELECTOR.KERNEL_DATA;

    // Update TSS descriptor
    gdt[5] = createDescriptor(tss_base, tss_limit, 0x89, 0x00);

    tss.io_permissions_base_offset = @sizeOf(TSS); // Disable I/O permission bitmap

    io.ltr(tss_selector);
}
