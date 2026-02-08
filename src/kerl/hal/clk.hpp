#pragma once
#include <cstdint>

#include "csrs.hpp"

namespace kerl::hal::csr::clk {
constexpr uintptr_t BASE = 0x40008000;

constexpr uintptr_t REFCTRL_OFFSET = 0x30;
constexpr uintptr_t REFCTRL_DIV_OFFSET = 0x34;
constexpr uintptr_t REFCTRL_SELECTED_OFFSET = 0x38;

constexpr uintptr_t SYSCTRL_OFFSET = 0x3C;
constexpr uintptr_t SYSCTRL_DIV_OFFSET = 0x40;
constexpr uintptr_t SYSCTRL_SELECTED_OFFSET = 0x44;

/** Valid clock sources for Ref Clock */
enum class ClkRefSrc: uint8_t {
    ROSC = 0,
    CLKSRC_CLK_REF_AUX = 1,
    XOSC = 2
};

struct RefCtrl {
    using src = csr::RegField<csr::ReadWrite, (BASE + REFCTRL_OFFSET), 0, 2>;
    using auxsrc = csr::RegField<csr::ReadWrite, (BASE + REFCTRL_OFFSET), 5, 2>;
};

struct RefSelected {
    using selected =
        csr::RegField<csr::ReadOnly, (BASE + REFCTRL_SELECTED_OFFSET), 0, 32>;
};

/** Valid clock sources for SYS Clock */
enum class ClkSysSrc: uint8_t {
    PLL_SYS = 0,
    ROSC = 2,
    XOSC,
    GPIO0,
    GPIO1
};

struct SysCtrl {
    using src = csr::RegField<csr::ReadWrite, (BASE + SYSCTRL_OFFSET), 0, 2>;
    using auxsrc = csr::RegField<csr::ReadWrite, (BASE + SYSCTRL_OFFSET), 5, 3>;
};

struct SysSelected {
    using selected =
        csr::RegField<csr::ReadOnly, (BASE + SYSCTRL_SELECTED_OFFSET), 0, 32>;
};

}  // namespace kerl::hal::clk
