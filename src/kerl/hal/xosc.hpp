#pragma once

#include <cstdint>

#include "csrs.hpp"

namespace kerl::hal::csr::xosc {
constexpr uintptr_t BASE = 0x40024000;
constexpr uintptr_t CTRL_OFFSET = 0x00;
constexpr uintptr_t STATUS_OFFSET = 0x04;
constexpr uintptr_t DORMANT_OFFSET = 0x08;
constexpr uintptr_t STARTUP_OFFSET = 0x0C;
constexpr uintptr_t COUNT_OFFSET = 0x1C;

constexpr uint16_t ENABLE_MAGIC = 0xFAB;
constexpr uint16_t DISABLE_MAGIC = 0xD1E;
constexpr uint16_t DEFAULT_FREQ_RANGE = 0xAA0;

struct Ctrl {
    using freq_range = RegField<ReadWrite, (BASE + CTRL_OFFSET), 0, 12>;
    using enable = RegField<ReadWrite, (BASE + CTRL_OFFSET), 12, 12>;
};

struct Status {
    using bad_write = RegField<ReadWrite, (BASE + STATUS_OFFSET), 24>;
    using stable = RegField<ReadOnly, (BASE + STATUS_OFFSET), 31>;
};
}  // namespace kerl::hal::csr::xosc
