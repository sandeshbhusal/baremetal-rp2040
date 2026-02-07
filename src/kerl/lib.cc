#include "hal/resets.hpp"

using namespace kerl::hal::csr::resets;

extern "C" void _start() {
    Reset::IO_Bank0::rmw(0);
    Reset::Pads_Bank0::rmw(0);

    while (ResetDone::Pads_Bank0::read() != 1 ||
           ResetDone::IO_Bank0::read() != 1);

    while (1);
}
