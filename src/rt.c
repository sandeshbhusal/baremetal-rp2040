#include <stdint.h>

/** Resets Registers **/
#define RESETS_BASE                 0x4000C000
#define RESETS_RESET_OFFSET         0x0
#define RESETS_RESET_DONE_OFFSET    0x8
#define QSPI_RESET_BIT_POS          (1 << 9) // Both reset and reset_done

/** SSI Pad Registers **/
#define PADS_QSPI_BASE          0x40020000
#define PADS_QSPI_VREF_OFFSET   0x00
#define PADS_QSPI_SCLK_OFFSET   0x04
#define PADS_QSPI_SD0_OFFSET    0x08
#define PADS_QSPI_SD1_OFFSET    0x0C
#define PADS_QSPI_SD2_OFFSET    0x10
#define PADS_QSPI_SD3_OFFSET    0x14
#define PADS_QSPI_SS_OFFSET     0x18
#define PADS_QSPI_VREF          (*(volatile uint32_t *)(PADS_QSPI_BASE + PADS_QSPI_VREF_OFFSET))
#define PADS_QSPI_SCLK          (*(volatile uint32_t *)(PADS_QSPI_BASE + PADS_QSPI_SCLK_OFFSET))
#define PADS_QSPI_SD0           (*(volatile uint32_t *)(PADS_QSPI_BASE + PADS_QSPI_SD0_OFFSET))
#define PADS_QSPI_SD1           (*(volatile uint32_t *)(PADS_QSPI_BASE + PADS_QSPI_SD1_OFFSET))
#define PADS_QSPI_SD2           (*(volatile uint32_t *)(PADS_QSPI_BASE + PADS_QSPI_SD2_OFFSET))
#define PADS_QSPI_SD3           (*(volatile uint32_t *)(PADS_QSPI_BASE + PADS_QSPI_SD3_OFFSET))
#define PADS_QSPI_SS            (*(volatile uint32_t *)(PADS_QSPI_BASE + PADS_QSPI_SS_OFFSET))

/** SSI Controller Registers **/
#define SSI_BASE            0x18000000
#define SSI_CTRLR0_OFFSET   0x00
#define SSI_ENR_OFFSET      0x08
#define SSI_BAUDR_OFFSET    0x14
#define SSI_SR_OFFSET       0x28
#define SSI_SPI_CTRLR0_OFFSET   0xF4

#define SSI_CTRLR0 (*(volatile uint32_t *)(SSI_BASE + SSI_CTRLR0_OFFSET))
#define SSI_ENR    (*(volatile uint32_t *)(SSI_BASE + SSI_ENR_OFFSET))
#define SSI_BAUDR  (*(volatile uint32_t *)(SSI_BASE + SSI_BAUDR_OFFSET))
#define SSI_SR     (*(volatile uint32_t *)(SSI_BASE + SSI_SR_OFFSET))
#define SSI_SPI_CTRLR0 (*(volatile uint32_t *)(SSI_BASE + SSI_SPI_CTRLR0_OFFSET))

struct QSPIPadConfig {
  uint32_t raw;
  struct {
    uint32_t slew_fast : 1;      // Slew rate (1=fast,0=slow)
    uint32_t en_schmitt : 1;     // Enable Schmitt Trigger
    uint32_t pull_down_en : 1;   // Pull Down Enable
    uint32_t pull_up_en : 1;     // Pull Up Enable
    uint32_t drive_strength : 2; // Drive Strength (0=2mA,1=4mA,2=8mA,3=12mA)
    uint32_t input_en : 1;       // Input Enable
    uint32_t output_dis : 1;     // Disable Output
    uint32_t reserved : 24;
  } bits;
};

struct QSPI_CTRLR0 {
  uint32_t raw;
  struct {
    uint32_t dfs : 4;             // Data Frame Size
    uint32_t frf : 2;             // Frame Format
    uint32_t scph : 1;            // Serial Clock Phase
    uint32_t scpol : 1;           // Serial Clock Polarity
    uint32_t tmod : 2;            // Transfer Mode
    uint32_t slv_oe : 1;          // Slave Output Enable
    uint32_t srl : 1;             // Shift Register Loop
    uint32_t cfs : 4;             // Control Frame Size
    uint32_t data_frame_size : 5; // Data Frame Size in 32b transfer mode
    uint32_t spi_frf : 2;         // SPI Frame Format (0=1-bit, 1=2-bit, 2=4-bit)
    uint32_t reserved : 1;
    uint32_t sste : 1;            // Slave Select Toggle Enable
    uint32_t reserved2 : 7;
  } bits;
};

struct SSI_SPI_CTRLR0_s {
  uint32_t raw;
  struct {
    uint32_t trans_type : 2;
    uint32_t addr_l : 4;
    uint32_t _reserved0 : 2;
    uint32_t inst_l : 2;
    uint32_t _reserved1 : 1;
    uint32_t wait_cycles : 5;
    uint32_t spi_ddr_en : 1;
    uint32_t inst_ddr_en : 1;
    uint32_t spi_rxds_en : 1;
    uint32_t _reserved2 : 5;
    uint32_t xip_cmd : 8;
  } bits;
};

__attribute__((section(".boot2"))) void __boot2() {
  // 1) Program the PHY
  // 1.a) Bring QSPI out of reset
  volatile uint32_t *resets_reset =
      (uint32_t *)(RESETS_BASE + RESETS_RESET_OFFSET);
  volatile uint32_t *resets_reset_done =
      (uint32_t *)(RESETS_BASE + RESETS_RESET_DONE_OFFSET);
  *resets_reset &= ~QSPI_RESET_BIT_POS; // Clear reset bit
  while ((*resets_reset_done & QSPI_RESET_BIT_POS) == 0)
    ; // Wait until reset done

  // 1.b) Configure the Pads for QSPI
  // - VREF set to 0x0 (reset is 0x0 but set this anyways)
  PADS_QSPI_VREF = 0x0;

  // 1. Configure SCLK (Output only, 8mA, Fast Slew, No Pulls)
  struct QSPIPadConfig sclk_config = {0};
  sclk_config.bits.slew_fast = 1;
  sclk_config.bits.drive_strength = 2; // 8mA
  sclk_config.bits.input_en = 0;
  PADS_QSPI_SCLK = sclk_config.raw;

  // 2. Configure SD0-SD3 (Input/Output, 8mA, Fast Slew, Pull-up, No Schmitt)
  struct QSPIPadConfig sd_config = {0};
  sd_config.bits.slew_fast = 1;
  sd_config.bits.en_schmitt = 0; // Disable Schmitt for speed
  sd_config.bits.pull_up_en = 1;
  sd_config.bits.drive_strength = 2;
  sd_config.bits.input_en = 1; // SD lines need input

  PADS_QSPI_SD0 = sd_config.raw;
  PADS_QSPI_SD1 = sd_config.raw;
  PADS_QSPI_SD2 = sd_config.raw;
  PADS_QSPI_SD3 = sd_config.raw;

  // 2) Initialize the SSI Controller for fast QSPI XIP Mode
  // More details in page 570
  // 2.a) Disable the SSI before configuration
  SSI_ENR   = 0x0; // Disable SSI
  SSI_BAUDR = 2; // Set clock divisor (clk_sys/2) for higher speed

  struct QSPI_CTRLR0 ctrlr0 = {0};
  // All settings from Page 599.
  ctrlr0.bits.frf = 0x2;            // Set to 4-bit QSPI mode
  ctrlr0.bits.data_frame_size = 31; // 32-bit data frames
  ctrlr0.bits.tmod = 0x3;
  SSI_CTRLR0 = ctrlr0.raw;

  // Settings from page 606
  struct SSI_SPI_CTRLR0_s ssi_spi_ctrlr0 = {0};
  // TODO: Verify with WinBond Specification.
  ssi_spi_ctrlr0.bits.xip_cmd = 0xEB;  // Quad I/O Fast Read command
  ssi_spi_ctrlr0.bits.inst_l = 2;      // 8-bit instruction
  ssi_spi_ctrlr0.bits.addr_l = 6;      // 24-bit address
  ssi_spi_ctrlr0.bits.trans_type = 1;  // Quad I/O Read???
  ssi_spi_ctrlr0.bits.wait_cycles = 4; // 6 wait cycles

  SSI_SPI_CTRLR0 = ssi_spi_ctrlr0.raw;
  
  // Test flash - do a read from address 0x100000
  volatile uint32_t *flash_xip_addr = (uint32_t *)0x10000000;
  uint32_t data = *flash_xip_addr;
  (void)data; // Prevent unused variable warning

  while (1)
    ;
}
