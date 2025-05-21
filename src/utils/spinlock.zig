const std = @import("std");

pub const SpinLock = struct {
    locked: bool,

    pub fn init() SpinLock {
        return SpinLock{
            .locked = false,
        };
    }

    pub fn lock(self: *SpinLock) void {
        self.locked = true;
    }


    pub fn release(self: @This()) void {
        self.locked = false;
    }
};
