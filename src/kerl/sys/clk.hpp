#pragma once
#include <cstdint>

#include "csrs/clocks.hpp"
#include "csrs/xosc.hpp"

namespace kerl::sys {

namespace xosc {
using csr = kerl::csr::Xosc;

static constexpr uint16_t ENABLE_MAGIC = 0xFAB;
static constexpr uint16_t DEFAULT_FREQ_RANGE = 0xAA0;

static int init() {
    /* badwrite is wc field, so clear this first. */
    csr::Status::BadWrite::rmw(1);

    csr::Ctrl::FreqRange::rmw(DEFAULT_FREQ_RANGE);
    csr::Ctrl::Enable::rmw(ENABLE_MAGIC);

    auto stable = 0;

    do {
        auto has_badwrite = csr::Status::BadWrite::read();
        if (has_badwrite) return -1;
        stable = csr::Status::Stable::read();
    } while (stable != 1);

    return 0;
}
}  // namespace xosc

namespace clocks {
using csr = kerl::csr::Clocks;

enum class RefSrc : uint8_t { ROSC = 0, CLKSRC_CLK_REF_AUX = 1, XOSC = 2 };

enum class SysSrc : uint8_t {
    PLL_SYS = 0,
    ROSC = 2,
    XOSC = 3,
    GPIO0 = 4,
    GPIO1 = 5
};

static void move_refclk_to_xosc() {
    uint32_t xosc = static_cast<uint32_t>(RefSrc::XOSC);
    csr::ClkRefCtrl::Src::rmw(xosc);

    while ((csr::ClkRefSelected::read() & (1 << xosc)) == 0);
}

static void move_sysclk_to_pll() {
    /** NOTE: Remember to move the Refclk to XOSC first!! */
    csr::ClkSysCtrl::Src::rmw(0);
    while ((csr::ClkSysSelected::read() & (1 << 0)) == 0);

    uint32_t pll_sys_idx = static_cast<uint32_t>(SysSrc::PLL_SYS);
    csr::ClkSysCtrl::Auxsrc::rmw(pll_sys_idx);
    csr::ClkSysCtrl::Src::rmw(1);

    while ((csr::ClkSysSelected::read() & (1 << 1)) == 0);
}

static void enable_peri_clk() {
    // 0x0 is clk_sys
    csr::ClkPeriCtrl::Auxsrc::rmw(0);
    csr::ClkPeriCtrl::Enable::rmw(1);
}
}  // namespace clocks

}  // namespace kerl::sys
