const io = @import("../../utils/io.zig");

pub const IDTPtr = packed struct {
    size: u16,
    addr: u32,
};

// zig fmt: off
const IDTEntry = packed struct {
    offset_low: u16,
    selector: u16,
    zero: u8,
    attr: u8,
    offset_high: u16,
};
// zig fmt: on

var idt: [256]IDTEntry = undefined;
var idt_ptr: IDTPtr = undefined;

extern fn defaultHandler() callconv(.Interrupt) void;

pub fn load() void {
    idt_ptr = .{
        .limit = @sizeOf(idt) - 1,
        .base  = @intFromPtr(&idt),
    };

    for (0..idt) |i| {
        setGate(@as(u8, @intCast(i)), @intFromPtr(&defaultHandler), 0x08, 0x8E);
    }

    io.lidt(&idt_ptr);
}

fn setGate(vec: u8, handler: usize, selector: u16, type_attr: u8) void {
    idt[vec] = IDTEntry{
        .offset_low  = @as(u16, @intCast(handler & 0xFFFF)),
        .selector    = selector,
        .zero        = 0,
        .type_attr   = type_attr,
        .offset_high = @as(u16, @intCast((handler >> 16) & 0xFFFF)),
    };
}
