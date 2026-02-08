#include "pll.hpp"

namespace kerl::sys {

void Pll::lock_pll_to_100mhz() {
    PWR::PD::rmw(1);  // Turn off PLL before configuration
    FBDiv::Div::rmw(100);  // Set feedback divider to 100
    ControlStatus::RefDiv::rmw(1);  // Set reference divider to 1 (use 12Mhz straight outta the XOSC)
    Prim::PostDiv1::rmw(6);  // Set post divider 1 to 6
    Prim::PostDiv2::rmw(2);  // Set post divider 2 to 2

    PWR::PD::rmw(0);  // Turn on PLL
    PWR::VcoPD::rmw(0);

    /* Wait for lock */
    while (ControlStatus::Lock::read() == 0) {
    }

    PWR::PostDivPD::rmw(0);
}

}
