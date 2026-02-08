#include "sys/drivers/uart.hpp"
#include "sys/gpio.hpp"
#include "sys/interrupts.hpp"
#include "sys/pll.hpp"
#include "sys/resets.hpp"
#include "sys/clk.hpp"

using namespace kerl;

void sysinit() {
    sys::Interrupts::disable_all();
    int start_crystal = sys::Xosc::init();
    if (start_crystal != 0) {
        while(1);
    }

    sys::Clocks::move_refclk_to_xosc();
    sys::Resets::pll_reset();
    sys::Pll::lock_pll_to_100mhz();
    sys::Clocks::move_sysclk_to_pll();
    sys::Clocks::enable_peri_clk();
    sys::Interrupts::enable_all();
}

extern "C" void _start() {
    using kerl::sys::drivers::PrimeCellUART;
    sysinit();

    sys::Resets::uart0_reset();

    // Mux GPIO0 -> UART0 TX, GPIO1 -> UART0 RX (function 2)
    sys::Resets::io_bank0_reset();
    sys::Resets::pads_bank0_reset();

    sys::IoBank0::Gpio<0>::set_function(kerl::sys::IoBank0::GpioPinFunction::FUNC_UART0);
    sys::IoBank0::Gpio<1>::set_function(kerl::sys::IoBank0::GpioPinFunction::FUNC_UART0);

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
