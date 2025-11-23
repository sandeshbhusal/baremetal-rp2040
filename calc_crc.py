#!/usr/bin/env python3
import sys


# Page 131 of the reference.
def rp2040_crc(data):
    polynomial = 0x04C11DB7
    crc = 0xFFFFFFFF

    for byte in data:
        crc ^= byte << 24
        for _ in range(8):
            if crc & 0x80000000:
                crc = (crc << 1) ^ polynomial
            else:
                crc = crc << 1
        crc &= 0xFFFFFFFF

    return crc


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input_bin> <output_c>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    with open(input_file, "rb") as f:
        data = f.read()

    if len(data) < 252:
        data = data + b"\x00" * (252 - len(data))
    elif len(data) > 252:
        data = data[:252]

    crc_data = data[:252]
    checksum = rp2040_crc(crc_data)

    # RP2040 is Little Endian, so we write LSB first
    with open(output_file, "w") as f:
        f.write('__attribute__((section(".crc"))) ')
        f.write(f"unsigned char crc[4] = {{")
        f.write(f"0x{(checksum >> 0) & 0xFF:02x}, ")
        f.write(f"0x{(checksum >> 8) & 0xFF:02x}, ")
        f.write(f"0x{(checksum >> 16) & 0xFF:02x}, ")
        f.write(f"0x{(checksum >> 24) & 0xFF:02x}")
        f.write("};\n")

    print(f"RP2040 CRC-32: 0x{checksum:08x}")


if __name__ == "__main__":
    main()
