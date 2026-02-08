#pragma once

namespace kerl::sys::interrupts {
inline void enable_all_interrupts() { __asm__ volatile("cpsie i"); }
inline void disable_all_interrupts() { __asm__ volatile("cpsid i"); }
}  // namespace kerl::sys::interrupts
