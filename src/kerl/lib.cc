#include "sys/interrupts.hpp"
#include "sys/pll.hpp"
#include "sys/resets.hpp"
#include "sys/sysclk.hpp"

using namespace kerl;

extern "C" void _start() {
    sys::interrupts::disable_all_interrupts();

    /** start the crystal oscillator first. */
    int start_crystal = sys::clk::init_xosc();
    if (start_crystal != 0) {
        // Handle error
        while(1);
    }

    /** Next, initialize the refclk to use the crystal oscillator */
    sys::clk::move_refclk_to_xosc();

    /** Next, bring PLLs out of reset */
    sys::resets::pll_reset();

    /** Next, initialize the PLLs and lock it onto XOSC */
    sys::pll::lock_pll_to_100mhz();
    
    /** Next, initialize the system clock on the PLLs */
    /** Next, turn on the board LED to show successful boot! */
    /** Next, print a message to the UART on UART 0 with a banner */
    /** Next, do a i2c read and print the value on the console */

    while (1);
}
