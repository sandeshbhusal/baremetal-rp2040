#include "hal/resets.hpp"

using namespace kerl::hal::csr::resets;

extern "C" void _start() {
    Reset::IO_Bank0::atomic_set(0);
    Reset::Pads_Bank0::atomic_set(0);

    while(1);
}
