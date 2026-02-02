#include <stdint.h>

extern uintptr_t _estack[];
extern void main();

// Registers
#define M0_BASE 0xE0000000
#define VTOR (volatile uint32_t*)(M0_BASE + 0xED08)

#define FLASH_BASE 0x10000000
#define VTOR_TABLE_ADDR (FLASH_BASE + 0x100)

#define RESETS_BASE 0x4000C000
#define RESETS_RESET (volatile uint32_t*)(RESETS_BASE + 0x0)
#define RESETS_RESET_DONE (volatile uint32_t*)(RESETS_BASE + 0x8)
#define RESETS_IO_QSPI (1 << 9)

#define PADS_QSPI_BASE 0x40020000
#define PADS_QSPI_SCLK (volatile uint32_t*)(PADS_QSPI_BASE + 0x04)
#define PADS_QSPI_SD0 (volatile uint32_t*)(PADS_QSPI_BASE + 0x08)
#define PADS_QSPI_SD1 (volatile uint32_t*)(PADS_QSPI_BASE + 0x0C)
#define PADS_QSPI_SD2 (volatile uint32_t*)(PADS_QSPI_BASE + 0x10)
#define PADS_QSPI_SD3 (volatile uint32_t*)(PADS_QSPI_BASE + 0x14)

#define SSI_BASE 0x18000000
#define SSI_CTRLR0 (volatile uint32_t*)(SSI_BASE + 0x00)
#define SSI_CTRLR1 (volatile uint32_t*)(SSI_BASE + 0x04)
#define SSI_SSIENR (volatile uint32_t*)(SSI_BASE + 0x08)
#define SSI_BAUDR (volatile uint32_t*)(SSI_BASE + 0x14)
#define SSI_SR (volatile uint32_t*)(SSI_BASE + 0x28)
#define SSI_DR0 (volatile uint32_t*)(SSI_BASE + 0x60)
#define SSI_RX_SAMPLE_DLY (volatile uint32_t*)(SSI_BASE + 0xF0)
#define SSI_SPI_CTRLR0 (volatile uint32_t*)(SSI_BASE + 0xF4)

// Values
#define SSI_SR_BUSY (1 << 0)
#define SSI_SR_TFE (1 << 2)  // Transmit FIFO Empty

// ----------------------------------------------------------------------------
// Unions for Register Access
// ----------------------------------------------------------------------------

typedef union {
    uint32_t raw;
    struct {
        uint32_t dfs : 4;
        uint32_t frf : 2;
        uint32_t scph : 1;
        uint32_t scpol : 1;
        uint32_t tmod : 2;
        uint32_t slv_oe : 1;
        uint32_t srl : 1;
        uint32_t cfs : 4;
        uint32_t dfs_32 : 5;
        uint32_t spi_frf : 2;
        uint32_t reserved : 9;
    } bits;
} ctrlr0_t;

typedef union {
    uint32_t raw;
    struct {
        uint32_t trans_type : 2;
        uint32_t addr_l : 4;
        uint32_t reserved0 : 2;
        uint32_t inst_l : 2;
        uint32_t reserved1 : 1;
        uint32_t wait_cycles : 5;
        uint32_t spi_ddr_en : 1;
        uint32_t inst_ddr_en : 1;
        uint32_t spi_rxds_en : 1;
        uint32_t reserved2 : 5;
        uint32_t xip_cmd : 8;
    } bits;
} spi_ctrlr0_t;

// ----------------------------------------------------------------------------
// VTOR Table Decls
// ----------------------------------------------------------------------------
void reset_handler() {
    extern uintptr_t _bss_start[];
    extern uintptr_t _bss_end[];

    volatile uintptr_t* bss = _bss_start;

    while (bss < _bss_end) *bss++ = 0;

    extern uintptr_t _data_start[];
    extern uintptr_t _data_end[];
    extern uintptr_t _data_load_addr[];

    volatile uintptr_t* data = _data_start;
    volatile uintptr_t* data_load = _data_load_addr;

    while (data < _data_end) {
        *data++ = *(uintptr_t*)data_load++;
    }

    main();
    while (1);
}

extern void nmi_handler() { while (1); }

extern void hardfault_handler() { while (1); }

const uintptr_t vtor_table[] __attribute__((section(".vtor"), aligned(256))) = {
    (uintptr_t)(_estack), (uintptr_t)(reset_handler), (uintptr_t)(nmi_handler),
    (uintptr_t)(hardfault_handler)};

// ----------------------------------------------------------------------------
// Boot2 Code
// ----------------------------------------------------------------------------

__attribute__((section(".boot2"))) void __boot2() {
    // 1. Bring QSPI IO out of reset
    *RESETS_RESET &= ~RESETS_IO_QSPI;
    while (!(*RESETS_RESET_DONE & RESETS_IO_QSPI));

    // 2. Configure Pads
    // SCLK: 8mA, Fast Slew, Input Enabled (for feedback)
    // SD0-3: 8mA, Fast Slew, Input Enabled, Schmitt Disabled
    // 0x61 = IE(1<<6) | DRIVE_8mA(2<<4) | SLEW_FAST(1<<0)
    *PADS_QSPI_SCLK = 0x61;
    *PADS_QSPI_SD0 = 0x61;
    *PADS_QSPI_SD1 = 0x61;
    *PADS_QSPI_SD2 = 0x61;
    *PADS_QSPI_SD3 = 0x61;

    // 3. Configure SSI Common
    *SSI_SSIENR = 0;         // Disable
    *SSI_BAUDR = 4;          // Clock Divisor
    *SSI_RX_SAMPLE_DLY = 1;  // 1 cycle sample delay (offset 0xF0)

    // 4. Perform Dummy Read to enter Continuous Read Mode (0xA0)
    // This tells the flash that subsequent commands won't have an instruction
    // prefix.

    // CTRLR0
    ctrlr0_t ctrlr0 = {0};
    ctrlr0.bits.frf = 0;      // Motorola SPI
    ctrlr0.bits.tmod = 3;     // EEPROM Read (Tx then Rx)
    ctrlr0.bits.dfs_32 = 31;  // 32-bit data frame
    ctrlr0.bits.spi_frf = 2;  // Quad SPI
    *SSI_CTRLR0 = ctrlr0.raw;

    // SPI_CTRLR0
    spi_ctrlr0_t spi_ctrl0 = {0};
    spi_ctrl0.bits.trans_type = 1;   // Cmd Serial, Addr Quad
    spi_ctrl0.bits.addr_l = 8;       // 8 nibbles (32b) -> 24b Address + 8b Mode
    spi_ctrl0.bits.inst_l = 2;       // 8-bit instruction
    spi_ctrl0.bits.wait_cycles = 4;  // 4 dummy clocks
    *SSI_SPI_CTRLR0 = spi_ctrl0.raw;

    *SSI_CTRLR1 = 0;  // NDF=0 (Single 32-bit read)

    *SSI_SSIENR = 1;  // Enable

    // Kick off the read
    *SSI_DR0 = 0xEB;        // Instruction: Fast Read Quad I/O
    *SSI_DR0 = 0x000000A0;  // Address (0) + Mode Bits (0xA0)

    // Wait for completion
    while ((*SSI_SR & SSI_SR_BUSY));

    // Flush FIFO (discard read data)
    (void)*SSI_DR0;

    // 5. Configure for XIP (Continuous Read)
    *SSI_SSIENR = 0;  // Disable

    // SPI_CTRLR0 for XIP
    spi_ctrl0.raw = 0;
    spi_ctrl0.bits.trans_type =
        2;                      // Addr Quad, Data Quad (No Instruction phase)
    spi_ctrl0.bits.addr_l = 8;  // 8 nibbles (32b) -> 24b Address + 8b Mode
    spi_ctrl0.bits.inst_l = 0;  // No instruction
    spi_ctrl0.bits.wait_cycles = 4;  // 4 dummy clocks
    spi_ctrl0.bits.xip_cmd =
        0xA0;  // Mode bits to send implicitly after address
    *SSI_SPI_CTRLR0 = spi_ctrl0.raw;

    *SSI_SSIENR = 1;  // Enable

    // At this point, SSI is in XIP mode with Flash configured
    // for winbond chip on the rp2040. We can proceed with setup.

    // Setup the VTOR register
    *VTOR = VTOR_TABLE_ADDR;

    uint32_t* vtor_ptr = (uint32_t*)VTOR_TABLE_ADDR;
    uint32_t stack_end = vtor_ptr[0];
    uint32_t reset_handler = vtor_ptr[1];

    // Jump to reset handler
    __asm__ volatile(
        "msr msp, %0\n\t"
        "bx %1"
        :
        : "r"(stack_end), "r"(reset_handler)
        : "memory");

    while (1);
}
