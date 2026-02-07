use core::arch::asm;


pub fn disable_all_interrupts() {
    unsafe {
        asm!("cpsid i");
    }
}

pub fn enable_all_interrupts() {
    unsafe {
        asm!("cpsie i");
    }
}
