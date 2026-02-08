#pragma once

#include <cstdint>

#include "csrs.hpp"

namespace kerl::hal::csr::pll {
using csr::ReadOnly;
using csr::ReadWrite;
using csr::RegField;

constexpr uintptr_t BASE = 0x40028000;
constexpr uintptr_t CS_OFFSET = 0x00;
constexpr uintptr_t PWR_OFFSET = 0x04;
constexpr uintptr_t FBDIV_OFFSET = 0x08;
constexpr uintptr_t PRIM_OFFSET = 0x0C;

struct ControlStatus {
    using RefDiv = RegField<ReadWrite, (BASE + CS_OFFSET), 0, 6>;
    using Bypass = RegField<ReadWrite, (BASE + CS_OFFSET), 8>;
    using Lock = RegField<ReadOnly, (BASE + CS_OFFSET), 31>;
};

struct PWR {
    using PD = RegField<ReadWrite, (BASE + PWR_OFFSET), 0>;
    using PostDivPD = RegField<ReadWrite, (BASE + PWR_OFFSET), 3>;
    using VcoPD = RegField<ReadWrite, (BASE + PWR_OFFSET), 5>;
};

struct FBDiv {
    using Div = RegField<ReadWrite, (BASE + FBDIV_OFFSET), 0, 12>;
};

struct Prim {
    using PostDiv1 = RegField<ReadWrite, (BASE + PRIM_OFFSET), 16, 3>;
    using PostDiv2 = RegField<ReadWrite, (BASE + PRIM_OFFSET), 12, 3>;
};

}  // namespace kerl::hal::pll
