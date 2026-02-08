#include "interrupts.hpp"

namespace kerl::sys {

void Interrupts::enable_all() { __asm__ volatile("cpsie i"); }
void Interrupts::disable_all() { __asm__ volatile("cpsid i"); }

} // namespace kerl::sys
