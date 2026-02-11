#pragma once
#include <cstdint>

#include "csrs/io_bank0.hpp"

namespace kerl::sys::gpio {

using csr = kerl::csr::IoBank0;

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

template <unsigned Pin>
struct Gpio {
    static_assert(Pin < 30, "Pin number must be less than 30");

    static void set_function(GpioPinFunction fn) {
        uint8_t func_value = static_cast<uint8_t>(fn);

        if constexpr (Pin == 0) {
            csr::Gpio0Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 1) {
            csr::Gpio1Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 2) {
            csr::Gpio2Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 3) {
            csr::Gpio3Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 4) {
            csr::Gpio4Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 5) {
            csr::Gpio5Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 6) {
            csr::Gpio6Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 7) {
            csr::Gpio7Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 8) {
            csr::Gpio8Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 9) {
            csr::Gpio9Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 10) {
            csr::Gpio10Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 11) {
            csr::Gpio11Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 12) {
            csr::Gpio12Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 13) {
            csr::Gpio13Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 14) {
            csr::Gpio14Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 15) {
            csr::Gpio15Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 16) {
            csr::Gpio16Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 17) {
            csr::Gpio17Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 18) {
            csr::Gpio18Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 19) {
            csr::Gpio19Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 20) {
            csr::Gpio20Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 21) {
            csr::Gpio21Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 22) {
            csr::Gpio22Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 23) {
            csr::Gpio23Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 24) {
            csr::Gpio24Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 25) {
            csr::Gpio25Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 26) {
            csr::Gpio26Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 27) {
            csr::Gpio27Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 28) {
            csr::Gpio28Ctrl::FuncSel::rmw(func_value);
        } else if constexpr (Pin == 29) {
            csr::Gpio29Ctrl::FuncSel::rmw(func_value);
        }
    }
};

}  // namespace kerl::sys::gpio
