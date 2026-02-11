#pragma once
#include "csrs/pll_sys.hpp"

namespace kerl::sys::pll {

using csr = kerl::csr::PllSys;

static void lock_pll_to_100mhz() {
    csr::Pwr::Pd::rmw(1);     // Turn off PLL before configuration
    csr::FbdivInt::rmw(100);  // Set feedback divider to 100
    csr::Cs::Refdiv::rmw(
        1);  // Set reference divider to 1 (use 12Mhz straight outta the XOSC)
    csr::Prim::Postdiv1::rmw(6);  // Set post divider 1 to 6
    csr::Prim::Postdiv2::rmw(2);  // Set post divider 2 to 2

    csr::Pwr::Pd::rmw(0);  // Turn on PLL
    csr::Pwr::Vcopd::rmw(0);

    /* Wait for lock */
    while (csr::Cs::Lock::read() == 0) {
    }

    csr::Pwr::PostDivPd::rmw(0);
}

}  // namespace kerl::sys::pll
