#pragma once

#include <cstdint>

namespace kerl::csr {
namespace Reset {
uintptr_t BASE = 0x4000c000;
uintptr_t RESET_OFFSET = 0x0;
uintptr_t RESET_WDOG_OFFSET = 0x4;
uintptr_t RESET_DONE_OFFSET = 0x8;

struct RegReset {
    uint32_t u;
    struct {
        uint8_t adc : 1;
        uint8_t busctrl : 1;
        uint8_t dma : 1;
        uint8_t i2c0 : 1;
        uint8_t i2c1 : 1;
        uint8_t io_bank0 : 1;
        uint8_t io_qspi : 1;
        uint8_t jtag : 1;
        uint8_t pads_bank0 : 1;
        uint8_t pads_qspi : 1;
        uint32_t __reserved : 22;
    } s;
};

struct RegResetDone {
    uint32_t u;
    struct {
        uint8_t adc : 1;
        uint8_t busctrl : 1;
        uint8_t dma : 1;
        uint8_t i2c0 : 1;
        uint8_t i2c1 : 1;
        uint8_t io_bank0 : 1;
        uint8_t io_qspi : 1;
        uint8_t jtag : 1;
        uint8_t pads_bank0 : 1;
        uint8_t pads_qspi : 1;
        uint32_t __reserved : 22;
    } s;
};

}  // namespace Reset
}  // namespace kerl::csr