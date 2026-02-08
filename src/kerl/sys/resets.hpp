#pragma once
#include <cstdint>
#include "csr.hpp"

namespace kerl::sys {

struct Resets {
    static constexpr uintptr_t BASE = 0x4000c000;
    static constexpr uintptr_t RESET_OFFSET = 0x0;
    static constexpr uintptr_t RESET_DONE = 0x8;

    template <typename P, uintptr_t A, unsigned Pos, unsigned W=1>
    using RF = kerl::sys::RegField<P, A, Pos, W>;
    using RO = kerl::sys::ReadOnly;
    using RW = kerl::sys::ReadWrite;

    struct Reset {
        using PLL_Sys = RF<RW, (BASE + RESET_OFFSET), 12>;
        using IO_Bank0 = RF<RW, (BASE + RESET_OFFSET), 5>;
        using Pads_Bank0 = RF<RW, (BASE + RESET_OFFSET), 8>;
        using Uart0 = RF<RW, (BASE + RESET_OFFSET), 22>;
        using Uart1 = RF<RW, (BASE + RESET_OFFSET), 23>;
    };

    struct ResetDone {
        using PLL_Sys = RF<RW, (BASE + RESET_DONE), 12>;
        using IO_Bank0 = RF<RO, (BASE + RESET_DONE), 5>;
        using Pads_Bank0 = RF<RO, (BASE + RESET_DONE), 8>;
        using Uart0 = RF<RO, (BASE + RESET_DONE), 22>;
        using Uart1 = RF<RO, (BASE + RESET_DONE), 23>;
    };

    static void pll_reset();
    static void io_bank0_reset();
    static void pads_bank0_reset();
    static void uart0_reset();
};

} // namespace kerl::sys
