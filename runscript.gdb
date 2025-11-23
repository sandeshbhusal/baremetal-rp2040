target extended-remote :3333

monitor rp2040.core0 configure -event gdb-detach reset
monitor reset halt
load
break __boot2

continue
