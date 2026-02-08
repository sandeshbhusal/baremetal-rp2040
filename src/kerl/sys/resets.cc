#include "resets.hpp"

namespace kerl::sys {

void Resets::pll_reset() {
    Resets::Reset::PLL_Sys::rmw(0);
    while (Resets::ResetDone::PLL_Sys::read() != 1);
}

void Resets::io_bank0_reset() {
    Resets::Reset::IO_Bank0::rmw(0);
    while (Resets::ResetDone::IO_Bank0::read() != 1);
}

void Resets::pads_bank0_reset() {
    Resets::Reset::Pads_Bank0::rmw(0);
    while (Resets::ResetDone::Pads_Bank0::read() != 1);
}

void Resets::uart0_reset() {
    Resets::Reset::Uart0::rmw(0);
    while (Resets::ResetDone::Uart0::read() != 1);
}

}
