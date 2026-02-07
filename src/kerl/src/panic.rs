//! A simple panic handler -> Does nothing for now.

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    // Maybe turn on the board LED and blink it, etc.
    loop {}
}
