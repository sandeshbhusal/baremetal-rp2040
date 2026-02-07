#pragma once

#include <cstdint>

#include "csrs.hpp"

namespace kerl::hal::csr::resets {
constexpr uintptr_t BASE = 0x4000c000;

constexpr uintptr_t RESET_OFFSET = 0x0;
constexpr uintptr_t WDSEL = 0x4;
constexpr uintptr_t RESET_DONE = 0x8;

struct Reset {
    using IO_Bank0 = RegField<ReadWrite, (BASE + RESET_OFFSET), 5>;
    using Pads_Bank0 = RegField<ReadWrite, (BASE + RESET_OFFSET), 8>;
};

struct ResetDone {
    using IO_Bank0 = RegField<ReadOnly, (BASE + RESET_DONE), 5>;
    using Pads_Bank0 = RegField<ReadOnly, (BASE + RESET_DONE), 8>;
};

}  // namespace kerl::hal::csr::resets
