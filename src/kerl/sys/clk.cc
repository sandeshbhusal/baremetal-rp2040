#include "clk.hpp"

namespace kerl::sys {

int Xosc::init() {
    /* badwrite is wc field, so clear this first. */
    Status::bad_write::rmw(1);

    Ctrl::freq_range::rmw(DEFAULT_FREQ_RANGE);
    Ctrl::enable::rmw(ENABLE_MAGIC);

    auto stable = 0;

    do {
        auto has_badwrite = Status::bad_write::read();
        if (has_badwrite) return -1;
        stable = Status::stable::read();
    } while (stable != 1);

    return 0;
}

void Clocks::move_refclk_to_xosc() {
    uint32_t xosc = static_cast<uint32_t>(RefSrc::XOSC);
    RefCtrl::src::rmw(xosc);

    while ((RefSelected::selected::read() & (1 << xosc)) == 0);
}

void Clocks::move_sysclk_to_pll() {
    /** NOTE: Remember to move the Refclk to XOSC first!! */
    SysCtrl::src::rmw(0); 
    while ((SysSelected::selected::read() & (1 << 0)) == 0);

    uint32_t pll_sys_idx = static_cast<uint32_t>(SysSrc::PLL_SYS);
    SysCtrl::auxsrc::rmw(pll_sys_idx);
    SysCtrl::src::rmw(1);

    while ((SysSelected::selected::read() & (1 << 1)) == 0);
}

void Clocks::enable_peri_clk() {
    // 0x0 is clk_sys
    PeriCtrl::auxsrc::rmw(0);
    PeriCtrl::enable::rmw(1);
}

}
