#pragma once

namespace kerl::sys {

struct Interrupts {
    static void enable_all();
    static void disable_all();
};

} // namespace kerl::sys
