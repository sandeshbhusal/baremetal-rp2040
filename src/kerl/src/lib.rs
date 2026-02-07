#![no_std]
#![no_main]

mod panic;
mod sys;

#[no_mangle]
pub extern "C" fn _start() -> ! {
    // Initializattion functions (not sure what else I need here)
    sys::interrupts::disable_all_interrupts();
    sys::sysclock::enable_xosc();

    // Enable all interrupts back
    sys::interrupts::enable_all_interrupts();

    loop {}
}
