#pragma once
#include <cstdint>
#include "csr.hpp"

namespace kerl::sys {

struct Pll {
    static constexpr uintptr_t BASE = 0x40028000;
    static constexpr uintptr_t CS_OFFSET = 0x00;
    static constexpr uintptr_t PWR_OFFSET = 0x04;
    static constexpr uintptr_t FBDIV_OFFSET = 0x08;
    static constexpr uintptr_t PRIM_OFFSET = 0x0C;

    template <typename P, uintptr_t A, unsigned Pos, unsigned W=1>
    using RF = kerl::sys::RegField<P, A, Pos, W>;
    using RO = kerl::sys::ReadOnly;
    using RW = kerl::sys::ReadWrite;

    struct ControlStatus {
        using RefDiv = RF<RW, (BASE + CS_OFFSET), 0, 6>;
        using Bypass = RF<RW, (BASE + CS_OFFSET), 8>;
        using Lock = RF<RO, (BASE + CS_OFFSET), 31>;
    };

    struct PWR {
        using PD = RF<RW, (BASE + PWR_OFFSET), 0>;
        using PostDivPD = RF<RW, (BASE + PWR_OFFSET), 3>;
        using VcoPD = RF<RW, (BASE + PWR_OFFSET), 5>;
    };

    struct FBDiv {
        using Div = RF<RW, (BASE + FBDIV_OFFSET), 0, 12>;
    };

    struct Prim {
        using PostDiv1 = RF<RW, (BASE + PRIM_OFFSET), 16, 3>;
        using PostDiv2 = RF<RW, (BASE + PRIM_OFFSET), 12, 3>;
    };

    static void lock_pll_to_100mhz();
};

} // namespace kerl::sys
