all:
	mkdir -p build/
	
	# Generate a raw binary from the boot2 sectio
	arm-none-eabi-gcc -c src/rt.c -o build/rt.o -Os -mthumb -mcpu=cortex-m0plus -ffreestanding -nostdlib -g
	arm-none-eabi-objcopy -O binary -j .boot2 build/rt.o build/rt.bin

	# Calculate the CRC and generate a .crc section to link into the elf
	python3 calc_crc.py build/rt.bin build/crc.c
	arm-none-eabi-gcc -c build/crc.c -o build/crc.o -Os -mthumb -mcpu=cortex-m0plus -ffreestanding -nostdlib -g

	# Generate the final image
	arm-none-eabi-ld build/rt.o build/crc.o -o build/image.elf -T rp2040.ld
	arm-none-eabi-objcopy -O binary build/image.elf build/image.bin

clean:
	rm -rf build || true

gdb:
	gdb-multiarch -x runscript.gdb build/image.elf --quiet