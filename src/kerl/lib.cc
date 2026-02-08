#include "sys/interrupts.hpp"
#include "sys/sysclk.hpp"

using namespace kerl;

extern "C" void _start() {
    sys::interrupts::disable_all_interrupts();

    /** start the crystal oscillator first. */
    int start_crystal = sys::sysclk::init_xosc();
    if (start_crystal != 0) {
        // Handle error
        while(1);
    }

    while (1);
}
