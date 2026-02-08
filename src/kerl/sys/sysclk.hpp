#pragma once

#include "../hal/clk.hpp"
#include "../hal/xosc.hpp"

namespace kerl::sys::clk {
using namespace kerl::hal::csr;

enum class ClkErr : uint8_t {
    BAD_WRITE = 1,
    STABLE_TIMEOUT = 2,
};

/**
 * @brief Initialize the crystal oscillator.
 *
 * @description The book mentions that ~1ms is OK for the 12Mhz
 * crystal oscillator, so we wait for 1ms. With a 12Mhz crystal oscillator,
 * this means we need to wait for 12,000,000 cycles. The counter in XOSC takes
 * 256 cycles per tick, so we program delay to 47 (1ms is 1000 cycles).
 */
inline int init_xosc() {
    /* badwrite is wc field, so clear this first. */
    xosc::Status::bad_write::rmw(1);

    xosc::Ctrl::freq_range::rmw(xosc::DEFAULT_FREQ_RANGE);
    xosc::Ctrl::enable::rmw(xosc::ENABLE_MAGIC);

    auto stable = 0;

    do {
        auto has_badwrite = xosc::Status::bad_write::read();
        if (has_badwrite) return -1;
        stable = xosc::Status::stable::read();
    } while (stable != 1);

    return 0;
}

inline void move_refclk_to_xosc() {
    uint32_t xosc = static_cast<uint32_t>(hal::csr::clk::ClkRefSrc::XOSC);
    hal::csr::clk::RefCtrl::src::rmw(xosc);

    while ((kerl::hal::csr::clk::RefSelected::selected::read() & (1 << xosc)) ==
           0);
}

/**
* After the XOSC is stable and the PLLs are locked to XOSC, we can switch the system clock to the PLL output.
*/
inline void move_sysclk_to_pll() {
    /** NOTE: Remember to move the Refclk to XOSC first!! */
    hal::csr::clk::SysCtrl::src::rmw(0); 
    while ((kerl::hal::csr::clk::SysSelected::selected::read() & (1 << 0)) == 0);

    uint32_t pll_sys_idx = static_cast<uint32_t>(hal::csr::clk::ClkSysSrc::PLL_SYS);
    hal::csr::clk::SysCtrl::auxsrc::rmw(pll_sys_idx);
    hal::csr::clk::SysCtrl::src::rmw(1);

    while ((kerl::hal::csr::clk::SysSelected::selected::read() & (1 << 1)) == 0);
}

}  // namespace kerl::sys::clk
