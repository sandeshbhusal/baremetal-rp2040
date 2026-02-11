#pragma once

namespace kerl::sys::interrupts {

void enable_all() { __asm__ volatile("cpsie i"); }
void disable_all() { __asm__ volatile("cpsid i"); }

} // namespace kerl::sys
