ENTRY(__boot2)

MEMORY {
    RAM(rwx) : ORIGIN = 0x20000000, LENGTH = 264K
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2M
    BOOT_RAM(rwx): ORIGIN = 0x20041F00, LENGTH = 256
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS {
    .boot2 : {
        __boot2_start__ = .;
        KEEP(*(.boot2*))
        . = __boot2_start__ + 252;
    } > BOOT_RAM AT > FLASH

    ASSERT(SIZEOF(.boot2) == 252, "Error: .boot2 section must be exactly 252 bytes!")

    .crc : {
        KEEP(*(.crc*))
        . = ORIGIN(FLASH) + 256;
    } > FLASH

    .vtor : {
        . = ORIGIN(FLASH) + 256;
        . = ALIGN(256);
        KEEP(*(.vtor*))
        . = ALIGN(4);
    } > FLASH

    .text : {
        KEEP(*(.text*))
        . = ALIGN(4);
        KEEP(*(.rodata*))
        . = ALIGN(4);
    } > FLASH

    .data : {
        KEEP(*(.data*))
        . = ALIGN(4);
    } > RAM AT > FLASH

    ASSERT(SIZEOF(.boot2) + SIZEOF(.crc) == 256, "Error: BL2 should be exactly 256 bytes!")
}
