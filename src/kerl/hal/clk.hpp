#pragma once
#include <cstdint>

#include "csrs.hpp"

namespace kerl::hal::csr::clk {
constexpr uintptr_t BASE = 0x40008000;

constexpr uintptr_t REFCTRL_OFFSET = 0x30;
constexpr uintptr_t REFCTRL_DIV_OFFSET = 0x34;
constexpr uintptr_t REFCTRL_SELECTED_OFFSET = 0x38;

/** These need to be different per-clock generator */
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

}  // namespace kerl::hal::clk
