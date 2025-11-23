#include <stdint.h>

__attribute__((section(".boot2")))
void __boot2() {
    volatile int x;
    volatile int *ptr = &x;
    // volatile int *ptr = 0;
    *ptr = 42;
    while(1);
}
