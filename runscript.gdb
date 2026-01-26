target extended-remote :3333

monitor rp2040.core0 configure -event gdb-detach reset
monitor program build/image.elf verify
monitor reset halt
break __boot2

continue
