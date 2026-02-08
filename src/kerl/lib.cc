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
    sys::Interrupts::enable_all();
}

extern "C" void _start() {
    sysinit();
    while(1);
}
