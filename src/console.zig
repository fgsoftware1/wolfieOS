const io = @import("utils/io.zig");

const VGA_WIDTH = 80;
const VGA_HEIGHT = 25;
const VGA_SIZE = VGA_WIDTH * VGA_HEIGHT;
const VGA_CTRL_REGISTER: u16 = 0x3D4;
const VGA_DATA_REGISTER: u16 = 0x3D5;
const VGA_CURSOR_HIGH: u8 = 14;
const VGA_CURSOR_LOW: u8 = 15;

pub const ConsoleColors = enum(u8) {
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Brown = 6,
    LightGray = 7,
    DarkGray = 8,
    LightBlue = 9,
    LightGreen = 10,
    LightCyan = 11,
    LightRed = 12,
    LightMagenta = 13,
    LightBrown = 14,
    White = 15,
};

var row: usize = 0;
var column: usize = 0;
var color = vgaEntryColor(ConsoleColors.LightGray, ConsoleColors.Blue);
var buffer = @as([*]volatile u16, @ptrFromInt(0xB8000));

// ANSI escape sequence handling
var escape_mode = false;
var csi_mode = false;
var escape_buffer: [32]u8 = undefined;
var escape_index: usize = 0;

fn enableCursor(cursor_start: u8, cursor_end: u8) void {
    io.outb(VGA_CTRL_REGISTER, 0x0A);
    io.outb(VGA_DATA_REGISTER, (io.inb(VGA_DATA_REGISTER) & 0xC0) | cursor_start);

    io.outb(VGA_CTRL_REGISTER, 0x0B);
    io.outb(VGA_DATA_REGISTER, (io.inb(VGA_DATA_REGISTER) & 0xE0) | cursor_end);
}

fn disableCursor() void {
    io.outb(VGA_CTRL_REGISTER, 0x0A);
    io.outb(VGA_DATA_REGISTER, 0x20);
}

fn updateCursor() void {
    const pos = row * VGA_WIDTH + column;

    io.outb(VGA_CTRL_REGISTER, VGA_CURSOR_LOW);
    io.outb(VGA_DATA_REGISTER, @as(u8, @truncate( pos & 0xFF)));

    io.outb(VGA_CTRL_REGISTER, VGA_CURSOR_HIGH);
    io.outb(VGA_DATA_REGISTER, @as(u8,@truncate((pos >> 8) & 0xFF)));
}

pub fn getCursorPosition() usize {
    var pos: usize = 0;

    io.outb(VGA_CTRL_REGISTER, VGA_CURSOR_HIGH);
    pos = ((@as(usize, io.inb(VGA_DATA_REGISTER))) << 8);

    io.outb(VGA_CTRL_REGISTER, VGA_CURSOR_LOW);
    pos |= @as(usize, io.inb(VGA_DATA_REGISTER));

    return pos;
}

pub fn initialize() void {
    clear();
    // Enable cursor with a block shape (start=0, end=15)
    enableCursor(0, 15);
    updateCursor();
}

fn vgaEntryColor(fg: ConsoleColors, bg: ConsoleColors) u8 {
    return @intFromEnum(fg) | (@intFromEnum(bg) << 4);
}

fn vgaEntry(uc: u8, new_color: u8) u16 {
    const c: u16 = new_color;

    return uc | (c << 8);
}

pub fn setColor(new_color: u8) void {
    color = new_color;
}

pub fn clear() void {
    for (buffer[0..VGA_SIZE]) |*cell| {
        cell.* = vgaEntry(' ', color);
    }
    row = 0;
    column = 0;
    updateCursor();
}

pub fn putCharAt(c: u8, new_color: u8, x: usize, y: usize) void {
    const index = y * VGA_WIDTH + x;
    buffer[index] = vgaEntry(c, new_color);
}

pub fn setCursorPosition(x: usize, y: usize) void {
    if (x < VGA_WIDTH and y < VGA_HEIGHT) {
        column = x;
        row = y;
        updateCursor();
    }
}

fn scrollUp() void {
    // Move all lines up by one
    for (1..VGA_HEIGHT) |y| {
        for (0..VGA_WIDTH) |x| {
            const src_index = y * VGA_WIDTH + x;
            const dst_index = (y - 1) * VGA_WIDTH + x;
            buffer[dst_index] = buffer[src_index];
        }
    }

    // Clear the last line
    for (0..VGA_WIDTH) |x| {
        putCharAt(' ', color, x, VGA_HEIGHT - 1);
    }
}

pub fn putChar(c: u8) void {
    // Handle escape sequences
    if (escape_mode) {
        handleEscapeSequence(c);
        return;
    }

    // Check for escape character
    if (c == 27) { // ESC character
        escape_mode = true;
        escape_index = 0;
        return;
    }

    // Handle newline
    if (c == '\n') {
        column = 0;
        row += 1;
        if (row >= VGA_HEIGHT) {
            scrollUp();
            row = VGA_HEIGHT - 1;
        }
        updateCursor();
        return;
    }

    // Handle carriage return
    if (c == '\r') {
        column = 0;
        updateCursor();
        return;
    }

    // Handle backspace
    if (c == 8) {
        if (column > 0) {
            column -= 1;
            putCharAt(' ', color, column, row);
        }
        updateCursor();
        return;
    }

    // Normal character
    putCharAt(c, color, column, row);
    column += 1;
    if (column == VGA_WIDTH) {
        column = 0;
        row += 1;
        if (row >= VGA_HEIGHT) {
            scrollUp();
            row = VGA_HEIGHT - 1;
        }
    }
    updateCursor();
}

fn handleEscapeSequence(c: u8) void {
    if (escape_index >= escape_buffer.len) {
        // Buffer overflow, reset escape mode
        escape_mode = false;
        csi_mode = false;
        return;
    }

    // Store the character
    escape_buffer[escape_index] = c;
    escape_index += 1;

    // Check for CSI sequence (ESC [)
    if (escape_index == 1 and c == '[') {
        csi_mode = true;
        return;
    }

    if (csi_mode) {
        // Check for end of CSI sequence
        if ((c >= 'A' and c <= 'Z') or (c >= 'a' and c <= 'z')) {
            processCsiSequence(c);
            escape_mode = false;
            csi_mode = false;
        }
    } else {
        // Handle simple escape sequences
        if (escape_index == 1) {
            // Simple escape sequence with one character
            processSimpleEscapeSequence(c);
            escape_mode = false;
        }
    }
}

fn processCsiSequence(final_byte: u8) void {
    // Parse parameters from escape_buffer
    var params: [4]usize = [_]usize{0} ** 4;
    var param_count: usize = 0;
    var current_param: usize = 0;

    // Start from index 1 to skip the '[' character
    for (1..escape_index - 1) |i| {
        const c = escape_buffer[i];
        if (c == ';') {
            if (param_count < params.len) {
                params[param_count] = current_param;
                param_count += 1;
                current_param = 0;
            }
        } else if (c >= '0' and c <= '9') {
            current_param = current_param * 10 + (c - '0');
        }
    }

    // Add the last parameter
    if (param_count < params.len) {
        params[param_count] = current_param;
        param_count += 1;
    }

    // Handle different CSI sequences
    switch (final_byte) {
        'A' => { // Cursor Up
            const n = if (param_count > 0 and params[0] > 0) params[0] else 1;
            if (row >= n) {
                row -= n;
            } else {
                row = 0;
            }
        },
        'B' => { // Cursor Down
            const n = if (param_count > 0 and params[0] > 0) params[0] else 1;
            row = @min(row + n, VGA_HEIGHT - 1);
        },
        'C' => { // Cursor Forward
            const n = if (param_count > 0 and params[0] > 0) params[0] else 1;
            column = @min(column + n, VGA_WIDTH - 1);
        },
        'D' => { // Cursor Back
            const n = if (param_count > 0 and params[0] > 0) params[0] else 1;
            if (column >= n) {
                column -= n;
            } else {
                column = 0;
            }
        },
        'H', 'f' => { // Cursor Position
            const y = if (param_count > 0) @max(params[0], 1) - 1 else 0;
            const x = if (param_count > 1) @max(params[1], 1) - 1 else 0;
            setCursorPosition(x, @min(y, VGA_HEIGHT - 1));
        },
        'J' => { // Erase in Display
            const n = if (param_count > 0) params[0] else 0;
            if (n == 2) {
                clear(); // Clear entire screen
            }
        },
        'K' => { // Erase in Line
            const n = if (param_count > 0) params[0] else 0;
            if (n == 0) { // Clear from cursor to end of line
                for (column..VGA_WIDTH) |x| {
                    putCharAt(' ', color, x, row);
                }
            } else if (n == 1) { // Clear from start of line to cursor
                for (0..column + 1) |x| {
                    putCharAt(' ', color, x, row);
                }
            } else if (n == 2) { // Clear entire line
                for (0..VGA_WIDTH) |x| {
                    putCharAt(' ', color, x, row);
                }
            }
        },
        'm' => { // Select Graphic Rendition (SGR)
            if (param_count == 0) {
                // Reset to default
                color = vgaEntryColor(ConsoleColors.LightGray, ConsoleColors.Blue);
            } else {
                for (0..param_count) |i| {
                    const param = params[i];

                    // Handle foreground colors
                    if (param >= 30 and param <= 37) {
                        const fg_color = @as(ConsoleColors, @enumFromInt(param - 30));
                        color = (color & 0xF0) | @intFromEnum(fg_color);
                    }
                    // Handle background colors
                    else if (param >= 40 and param <= 47) {
                        const bg_color = @as(ConsoleColors, @enumFromInt(param - 40));
                        color = (color & 0x0F) | (@intFromEnum(bg_color) << 4);
                    }
                    // Handle bright foreground colors
                    else if (param >= 90 and param <= 97) {
                        const fg_color = @as(ConsoleColors, @enumFromInt((param - 90) + 8));
                        color = (color & 0xF0) | @intFromEnum(fg_color);
                    }
                    // Handle bright background colors
                    else if (param >= 100 and param <= 107) {
                        const bg_color = @as(ConsoleColors, @enumFromInt((param - 100) + 8));
                        color = (color & 0x0F) | (@intFromEnum(bg_color) << 4);
                    }
                }
            }
        },
        else => {
            // Unhandled CSI sequence
        },
    }
}

fn processSimpleEscapeSequence(c: u8) void {
    switch (c) {
        'c' => { // Reset terminal
            clear();
            color = vgaEntryColor(ConsoleColors.LightGray, ConsoleColors.Blue);
        },
        // Add more simple escape sequences as needed
        else => {
            // Unhandled escape sequence
        },
    }
}

pub fn puts(data: []const u8) void {
    for (data) |c| {
        putChar(c);
    }
}

pub fn getCursorRow() usize {
    return row;
}

pub fn getCursorColumn() usize {
    return column;
}

pub fn setCursorStyle(start: u8, end: u8) void {
    enableCursor(start, end);
}

pub fn setCursorVisible(visible: bool) void {
    if (visible) {
        // Default block cursor
        enableCursor(0, 15);
    } else {
        disableCursor();
    }
}

// Move cursor functions
pub fn cursorUp(count: usize) void {
    if (row >= count) {
        row -= count;
    } else {
        row = 0;
    }
    updateCursor();
}

pub fn cursorDown(count: usize) void {
    row = @min(row + count, VGA_HEIGHT - 1);
    updateCursor();
}

pub fn cursorForward(count: usize) void {
    column = @min(column + count, VGA_WIDTH - 1);
    updateCursor();
}

pub fn cursorBackward(count: usize) void {
    if (column >= count) {
        column -= count;
    } else {
        column = 0;
    }
    updateCursor();
}

pub fn saveCursorPosition() struct { row: usize, column: usize } {
    return .{ .row = row, .column = column };
}

pub fn restoreCursorPosition(saved: struct { row: usize, column: usize }) void {
    row = saved.row;
    column = saved.column;
    updateCursor();
}
