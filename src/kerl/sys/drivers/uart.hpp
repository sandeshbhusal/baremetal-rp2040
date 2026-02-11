#pragma once
#include <cstdint>
#include "../csr.hpp"

namespace kerl::sys::drivers {

template <uintptr_t BASE>
struct PrimeCellUART {
    static constexpr uintptr_t DR_OFFSET = 0x0;
    static constexpr uintptr_t RSR_OFFSET = 0x4;
    static constexpr uintptr_t FR_OFFSET = 0x18;
    static constexpr uintptr_t ILPR_OFFSET = 0x20;
    static constexpr uintptr_t IBRD_OFFSET = 0x24;
    static constexpr uintptr_t FBRD_OFFSET = 0x28;
    static constexpr uintptr_t LCRH_OFFSET = 0x2C;
    static constexpr uintptr_t CR_OFFSET = 0x30;
    static constexpr uintptr_t IFLS_OFFSET = 0x34;
    static constexpr uintptr_t IMSC_OFFSET = 0x38;
    static constexpr uintptr_t RIS_OFFSET = 0x3C;
    static constexpr uintptr_t MIS_OFFSET = 0x40;
    static constexpr uintptr_t ICR_OFFSET = 0x44;
    static constexpr uintptr_t DMACR_OFFSET = 0x48;
    static constexpr uintptr_t PERIPHID0_OFFSET = 0xFE0;
    static constexpr uintptr_t PERIPHID1_OFFSET = 0xFE4;
    static constexpr uintptr_t PERIPHID2_OFFSET = 0xFE8;
    static constexpr uintptr_t PERIPHID3_OFFSET = 0xFEC;
    static constexpr uintptr_t PCELLID0_OFFSET = 0xFF0;
    static constexpr uintptr_t PCELLID1_OFFSET = 0xFF4;
    static constexpr uintptr_t PCELLID2_OFFSET = 0xFF8;
    static constexpr uintptr_t PCELLID3_OFFSET = 0xFFC;

    template <typename P, uintptr_t A, unsigned Pos, unsigned W=1>
    using RF = kerl::csr::RegField<P, A, Pos, W>;
    using RO = kerl::csr::ReadOnly;
    using RW = kerl::csr::ReadWrite;
    using WO = kerl::csr::WriteOnly;

    struct DataReg {
        using Data = RF<RW, (BASE + DR_OFFSET), 0, 8>;
        using FrameError = RF<RO, (BASE + DR_OFFSET), 8>;
        using ParityError = RF<RO, (BASE + DR_OFFSET), 9>;
        using BreakError = RF<RO, (BASE + DR_OFFSET), 10>;
        using OverrunError = RF<RO, (BASE + DR_OFFSET), 11>;
    };

    struct ReceiveStatusReg {
        using FramingError = RF<RO, (BASE + RSR_OFFSET), 0>;
        using ParityError = RF<RO, (BASE + RSR_OFFSET), 1>;
        using BreakError = RF<RO, (BASE + RSR_OFFSET), 2>;
        using OverrunError = RF<RO, (BASE + RSR_OFFSET), 3>;
    };

    struct FlagReg {
        using ClearToSend = RF<RO, (BASE + FR_OFFSET), 0>;
        using DataSetReady = RF<RO, (BASE + FR_OFFSET), 1>;
        using DataCarrierDetect = RF<RO, (BASE + FR_OFFSET), 2>;
        using Busy = RF<RO, (BASE + FR_OFFSET), 3>;
        using RxFIFOEmpty = RF<RO, (BASE + FR_OFFSET), 4>;
        using TxFIFOFull = RF<RO, (BASE + FR_OFFSET), 5>;
        using RxFIFOFull = RF<RO, (BASE + FR_OFFSET), 6>;
        using TxFIFOEmpty = RF<RO, (BASE + FR_OFFSET), 7>;
        using RingIndicator = RF<RO, (BASE + FR_OFFSET), 8>;
    };

    struct IntegerBaudReg {
        using DivInt = RF<RW, (BASE + IBRD_OFFSET), 0, 16>;
    };

    struct FractionalBaudReg {
        using DivFrac = RF<RW, (BASE + FBRD_OFFSET), 0, 6>;
    };

    struct LineCtrlReg {
        using Break = RF<RW, (BASE + LCRH_OFFSET), 0, 1>;
        using ParityEn = RF<RW, (BASE + LCRH_OFFSET), 1, 1>;
        using EvenParity = RF<RW, (BASE + LCRH_OFFSET), 2, 1>;
        using TwoStopBitsSelect = RF<RW, (BASE + LCRH_OFFSET), 3, 1>;
        using FifoEnable = RF<RW, (BASE + LCRH_OFFSET), 4, 1>;
        using WordLength = RF<RW, (BASE + LCRH_OFFSET), 5, 2>;
        using StickyParitySet = RF<RW, (BASE + LCRH_OFFSET), 7, 1>;
    };

    struct ControlReg {
        using UartEn = RF<RW, (BASE + CR_OFFSET), 0, 1>;
        using LoopBackEn = RF<RW, (BASE + CR_OFFSET), 7, 1>;
        using TxEn = RF<RW, (BASE + CR_OFFSET), 8, 1>;
        using RxEn = RF<RW, (BASE + CR_OFFSET), 9, 1>;
        using DataTxReady = RF<RW, (BASE + CR_OFFSET), 10, 1>;
        using RequestToSend = RF<RW, (BASE + CR_OFFSET), 11, 1>;
        using Out1 = RF<RW, (BASE + CR_OFFSET), 12, 1>;
        using Out2 = RF<RW, (BASE + CR_OFFSET), 13, 1>;
        using ReadyToSendEn = RF<RW, (BASE + CR_OFFSET), 14, 1>;
        using ClearToSendEn = RF<RW, (BASE + CR_OFFSET), 15, 1>;
    };

    struct InterruptFifoLevelSelectReg {
        using TxIFLSel = RF<RW, (BASE + IFLS_OFFSET), 0, 3>;
        using RxIFLSel = RF<RW, (BASE + IFLS_OFFSET), 3, 3>;
    };

    struct InterruptMaskReg {
        using RIMIM = RF<RW, (BASE + IMSC_OFFSET), 0, 1>;
        using CTSMIM = RF<RW, (BASE + IMSC_OFFSET), 1, 1>;
        using DCDMIM = RF<RW, (BASE + IMSC_OFFSET), 2, 1>;
        using DSRMIM = RF<RW, (BASE + IMSC_OFFSET), 3, 1>;
        using RXMIM = RF<RW, (BASE + IMSC_OFFSET), 4, 1>;
        using TXMIM = RF<RW, (BASE + IMSC_OFFSET), 5, 1>;
        using RTMIM = RF<RW, (BASE + IMSC_OFFSET), 6, 1>;
        using FEMIM = RF<RW, (BASE + IMSC_OFFSET), 7, 1>;
        using PEMIM = RF<RW, (BASE + IMSC_OFFSET), 8, 1>;
        using BEMIM = RF<RW, (BASE + IMSC_OFFSET), 9, 1>;
        using OEMIM = RF<RW, (BASE + IMSC_OFFSET), 10, 1>;
    };


    PrimeCellUART(uint32_t baud_rate, uint32_t peri_clk_hz = 100000000) {
        disable();
        set_baudrate(baud_rate, peri_clk_hz);
        LineCtrlReg::WordLength::rmw(0b11);  // 8 data bits
        enable_fifos();
        enable();
        enable_tx_rx();
    }

    inline void disable() {
        ControlReg::UartEn::rmw(0);
    }

    inline void enable() {
        ControlReg::UartEn::rmw(1);
    }

    inline void set_baudrate(uint32_t baud_rate, uint32_t peri_clk_hz) {
        uint32_t ibrd = peri_clk_hz / (16 * baud_rate);
        uint32_t remainder = peri_clk_hz % (16 * baud_rate);
        uint32_t fbrd = (remainder * 64 + (16 * baud_rate) / 2) / (16 * baud_rate);
        IntegerBaudReg::DivInt::rmw(ibrd);
        FractionalBaudReg::DivFrac::rmw(fbrd);
    }

    inline void enable_fifos() {
        LineCtrlReg::FifoEnable::rmw(1);
    }

    inline void enable_tx_rx() {
        ControlReg::TxEn::rmw(1);
        ControlReg::RxEn::rmw(1);
    }

    void write_char(char c) {
        while (FlagReg::TxFIFOFull::read() == 1) {
        }
        DataReg::Data::rmw(c);
    }

    char read_char() {
        while (FlagReg::RxFIFOEmpty::read() == 1) {
        }
        return static_cast<char>(DataReg::Data::read());
    }

    void write_str(const char* str) {
        while (*str) {
            write_char(*str++);
        }
    }
};

}  // namespace kerl::sys::drivers
