#include "sys/drivers/uart.hpp"
#include "sys/gpio.hpp"
#include "sys/interrupts.hpp"
#include "sys/pll.hpp"
#include "sys/resets.hpp"
#include "sys/clk.hpp"

using namespace kerl;

void sysinit() {
    sys::interrupts::disable_all();
    int start_crystal = sys::xosc::init();
    if (start_crystal != 0) {
        while(1);
    }

    sys::clocks::move_refclk_to_xosc();
    sys::resets::pll_reset();
    sys::pll::lock_pll_to_100mhz();
    sys::clocks::move_sysclk_to_pll();
    sys::clocks::enable_peri_clk();
    sys::interrupts::enable_all();
}

extern "C" void _start() {
    using kerl::sys::drivers::PrimeCellUART;
    sysinit();

    sys::resets::uart0_reset();

    // Mux GPIO0 -> UART0 TX, GPIO1 -> UART0 RX (function 2)
    sys::resets::io_bank0_reset();
    sys::resets::pads_bank0_reset();

    sys::gpio::Gpio<0>::set_function(sys::gpio::GpioPinFunction::FUNC_UART0);
    sys::gpio::Gpio<1>::set_function(sys::gpio::GpioPinFunction::FUNC_UART0);

    PrimeCellUART<0x40034000> uart0(115200);
    // Poor attempt at CLS
    for (int i = 0; i < 32; i++)
        uart0.write_str("\r\n");

    uart0.write_str("\r\n");
    uart0.write_str("Kerl v0\r\n");
    uart0.write_str("-------------------------\r\n");
    uart0.write_str("Running on Raspberry Pi Pico!\r\n");
    uart0.write_str("Kerl is an experimental embedded runtime written in C++\r\n");
    uart0.write_str("(c) 2026 Sandesh Bhusal\r\n");
    uart0.write_str("Licensed under No License. Do whatever you want with it!\r\n");

    for (int i = 0; i < 5; i++)
        uart0.write_str("\r\n");


    while(1);
}
