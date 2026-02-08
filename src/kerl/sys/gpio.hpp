#pragma once
#include <cstdint>
#include "csr.hpp"

namespace kerl::sys {

struct IoBank0 {
    static constexpr uintptr_t BASE = 0x40014000;

    enum class GpioPinFunction {
        FUNC_XIP = 0,
        FUNC_SPI0 = 1,
        FUNC_SPI1 = 1,
        FUNC_I2C0 = 1,
        FUNC_I2C1 = 1,
        FUNC_UART0 = 2,
        FUNC_UART1 = 2,
        FUNC_PWM = 3,
        FUNC_SIO = 5,
        FUNC_PIO0 = 6,
        FUNC_PIO1 = 7,
        FUNC_CLK_REF = 8,
        FUNC_USB_ALT = 9
    };

    template <typename P, uintptr_t A, unsigned Pos, unsigned W=1>
    using RF = kerl::sys::RegField<P, A, Pos, W>;
    using RW = kerl::sys::ReadWrite;

    template <unsigned Pin>
    struct Gpio {
        static_assert(Pin < 30, "Pin number must be less than 30");
        
        // GPIOx_STATUS is at offset Pin * 8
        // GPIOx_CTRL is at offset Pin * 8 + 4
        
        struct Ctrl {
            static constexpr uintptr_t ADDR = BASE + (Pin * 8) + 4;
            using FuncSel = RF<RW, ADDR, 0, 5>;
            using OutOver = RF<RW, ADDR, 12, 2>;
            using OeOver  = RF<RW, ADDR, 14, 2>;
            using InOver  = RF<RW, ADDR, 16, 2>;
        };

        static void set_function(GpioPinFunction fn) {
            Ctrl::FuncSel::rmw(static_cast<uint8_t>(fn));
        }
    };
};

} // namespace kerl::sys
