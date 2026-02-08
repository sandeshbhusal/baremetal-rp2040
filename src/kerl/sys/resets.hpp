#pragma once

#include "../hal/resets.hpp"

namespace kerl::sys::resets {

void pll_reset() {
    kerl::hal::csr::resets::Reset::PLL_Sys::rmw(0);
    while (kerl::hal::csr::resets::ResetDone::PLL_Sys::read() != 1);
}

}  // namespace kerl::sys::resets
