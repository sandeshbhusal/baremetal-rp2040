#pragma once

#include "../hal/pll.hpp"

namespace kerl::sys::pll {

inline void lock_pll_to_100mhz() {
    kerl::hal::csr::pll::PWR::PD::rmw(1);  // Turn off PLL before configuration

    /* Im going to use a 100 Mhz configuration for the PLL_SYS */
    kerl::hal::csr::pll::FBDiv::Div::rmw(100);  // Set feedback divider to 100

    kerl::hal::csr::pll::ControlStatus::RefDiv::rmw(
        1);  // Set reference divider to 1 (use 12Mhz straight outta the XOSC)

    kerl::hal::csr::pll::Prim::PostDiv1::rmw(6);  // Set post divider 1 to 6
    kerl::hal::csr::pll::Prim::PostDiv2::rmw(2);  // Set post divider 2 to 2

    /**
     * At this point, 12 / 1 * 100 / 6 / 2 = 100 Mhz should be the output
     * frequency of the PLL. Now we can turn on the PLL and wait for it to lock.
     */

    kerl::hal::csr::pll::PWR::PD::rmw(0);  // Turn on PLL

    /* Wait for lock */
    while (kerl::hal::csr::pll::ControlStatus::Lock::read() == 0) {
    }
}

}  // namespace kerl::sys::pll
