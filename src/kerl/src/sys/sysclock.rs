use core::ptr::{read_volatile, write_volatile};

// 1. Define the Register Block
// #[repr(C)] ensures the compiler lays out the struct exactly as defined,
// without padding or reordering (provided the fields are naturally aligned).
#[repr(C)]
pub struct XoscBlock {
    pub ctrl: u32,    // Offset 0x00
    pub status: u32,  // Offset 0x04
    pub dormant: u32, // Offset 0x08
    pub startup: u32, // Offset 0x0C
                      // If there were gaps, you would use: _reserved: [u8; gap_size]
}

// 2. Define Constants for Bit Manipulation
// These are still necessary, but now they are grouped logically.
impl XoscBlock {
    const CTRL_ENABLE_MAGIC: u32 = 0xFAB << 12;
    const CTRL_FREQ_RANGE_1_15MHZ: u32 = 0xAA0;
    const STATUS_STABLE_BIT: u32 = 1 << 31;
    const STARTUP_DELAY_1MS: u32 = 47; // Calculated for 12MHz
}

// 3. Define the Base Address
// We cast this raw integer to a pointer to our struct.
const XOSC_BASE: *mut XoscBlock = 0x40024000 as *mut XoscBlock;

pub fn enable_xosc() {
    unsafe {
        // Dereferencing a raw pointer is unsafe.
        // We use reference notation for readability, but behind the scenes
        // we must strictly use volatile accesses for MMIO.

        let xosc = &mut *XOSC_BASE;

        // A. Set Startup Delay
        // Equivalent to: write_volatile(0x4002400C, 47)
        write_volatile(&mut xosc.startup, XoscBlock::STARTUP_DELAY_1MS);

        // B. Enable XOSC
        // Equivalent to: write_volatile(0x40024000, 0xFABAA0)
        let ctrl_val = XoscBlock::CTRL_ENABLE_MAGIC | XoscBlock::CTRL_FREQ_RANGE_1_15MHZ;
        write_volatile(&mut xosc.ctrl, ctrl_val);

        // C. Wait for Stability
        loop {
            // Equivalent to: read_volatile(0x40024004)
            let status = read_volatile(&xosc.status);
            if (status & XoscBlock::STATUS_STABLE_BIT) != 0 {
                break;
            }
        }
    }
}
