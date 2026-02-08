#pragma once

#include <expected>
#include "../hal/xosc.hpp"

namespace kerl::sys::sysclk {
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
    /* badwrite is wc field */
    // xosc::Status::bad_write::rmw(1);
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

}  // namespace kerl::sys::sysclk
