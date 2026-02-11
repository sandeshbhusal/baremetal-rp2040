#pragma once
#include "csrs/resets.hpp"

namespace kerl::sys::resets {

using csr = kerl::csr::Resets;

static void pll_reset() {
    csr::Reset::PllSys::rmw(0);
    while (csr::ResetDone::PllSys::read() != 1);
}

static void io_bank0_reset() {
    csr::Reset::IoBank0::rmw(0);
    while (csr::ResetDone::IoBank0::read() != 1);
}

static void pads_bank0_reset() {
    csr::Reset::PadsBank0::rmw(0);
    while (csr::ResetDone::PadsBank0::read() != 1);
}

static void uart0_reset() {
    csr::Reset::Uart0::rmw(0);
    while (csr::ResetDone::Uart0::read() != 1);
}

}  // namespace kerl::sys
