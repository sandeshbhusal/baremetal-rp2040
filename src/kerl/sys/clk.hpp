#pragma once
#include <cstdint>
#include "csr.hpp"

namespace kerl::sys {

struct Xosc {
    static constexpr uintptr_t BASE = 0x40024000;
    static constexpr uintptr_t CTRL_OFFSET = 0x00;
    static constexpr uintptr_t STATUS_OFFSET = 0x04;
    
    static constexpr uint16_t ENABLE_MAGIC = 0xFAB;
    static constexpr uint16_t DEFAULT_FREQ_RANGE = 0xAA0;

    template <typename P, uintptr_t A, unsigned Pos, unsigned W=1>
    using RF = kerl::sys::RegField<P, A, Pos, W>;
    using RO = kerl::sys::ReadOnly;
    using RW = kerl::sys::ReadWrite;

    struct Ctrl {
        using freq_range = RF<RW, (BASE + CTRL_OFFSET), 0, 12>;
        using enable = RF<RW, (BASE + CTRL_OFFSET), 12, 12>;
    };

    struct Status {
        using bad_write = RF<RW, (BASE + STATUS_OFFSET), 24>;
        using stable = RF<RO, (BASE + STATUS_OFFSET), 31>;
    };

    static int init();
};

struct Clocks {
    enum class RefSrc: uint8_t {
        ROSC = 0,
        CLKSRC_CLK_REF_AUX = 1,
        XOSC = 2
    };

    enum class SysSrc: uint8_t {
        PLL_SYS = 0,
        ROSC = 2,
        XOSC = 3,
        GPIO0 = 4,
        GPIO1 = 5
    };

    static constexpr uintptr_t BASE = 0x40008000;
    static constexpr uintptr_t REFCTRL_OFFSET = 0x30;
    static constexpr uintptr_t REFCTRL_SELECTED_OFFSET = 0x38;
    static constexpr uintptr_t SYSCTRL_OFFSET = 0x3C;
    static constexpr uintptr_t SYSCTRL_SELECTED_OFFSET = 0x44;

    template <typename P, uintptr_t A, unsigned Pos, unsigned W=1>
    using RF = kerl::sys::RegField<P, A, Pos, W>;
    using RO = kerl::sys::ReadOnly;
    using RW = kerl::sys::ReadWrite;
    
    struct RefCtrl {
        using src = RF<RW, (BASE + REFCTRL_OFFSET), 0, 2>;
        using auxsrc = RF<RW, (BASE + REFCTRL_OFFSET), 5, 2>;
    };

    struct RefSelected {
        using selected = RF<RO, (BASE + REFCTRL_SELECTED_OFFSET), 0, 32>;
    };

    struct SysCtrl {
        using src = RF<RW, (BASE + SYSCTRL_OFFSET), 0, 2>;
        using auxsrc = RF<RW, (BASE + SYSCTRL_OFFSET), 5, 3>;
    };

    struct SysSelected {
        using selected = RF<RO, (BASE + SYSCTRL_SELECTED_OFFSET), 0, 32>;
    };

    static void move_refclk_to_xosc();
    static void move_sysclk_to_pll();
};

} // namespace kerl::sys
