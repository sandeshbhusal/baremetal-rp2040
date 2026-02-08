#pragma once

#include <cstdint>

namespace kerl::sys {

/* Marker types for fields */
struct ReadOnly {
    static constexpr bool can_read = true;
    static constexpr bool can_write = false;
    static constexpr bool is_atomic = false;
};

struct WriteOnly {
    static constexpr bool can_read = false;
    static constexpr bool can_write = true;
    static constexpr bool is_atomic = false;
};

struct ReadWrite {
    static constexpr bool can_read = true;
    static constexpr bool can_write = true;
    static constexpr bool is_atomic = false;
};

struct AtomicWrite {
    static constexpr bool can_read = true;
    static constexpr bool can_write = true;
    static constexpr bool is_atomic = true;
};

/**
 * @brief: Register field access struct
 */
template <typename Policy, std::uintptr_t REG_ADDRESS, unsigned position,
          unsigned width = 1>
struct RegField {
    static constexpr uintptr_t MASK =
        (width >= 32) ? 0xFFFFFFFFu : ((1u << width) - 1u);

    /**
     * @brief: Read-Modify-Write a register field
     *
     * @description: Only affects the space this field is occupying in the register
     */
    static void rmw(std::uint32_t value) {
        static_assert(Policy::can_read && Policy::can_write,
                      "Cannot either read/write to this reg");

        volatile uintptr_t* reg_addr = (volatile uintptr_t*)(REG_ADDRESS);
        auto set_value = *reg_addr;
        set_value = set_value & (~(MASK << position));
        set_value |= ((value & MASK) << position);

        *reg_addr = set_value;
    }

    /**
     * @brief: Read the register field value
     */
    static uint32_t read() {
        static_assert(Policy::can_read, "Cannot read this reg");
        return (*((volatile uintptr_t*)(REG_ADDRESS)) >> position) & MASK;
    }

    /**
     * @brief: Set the register automically. The register needs to be atomic for
     * this.
     */
    static void atomic_set(std::uint32_t value = 1) {
        static_assert(Policy::is_atomic && Policy::can_write,
                      "Not atomic/can't write to this reg");
    }

    /**
     * @brief: Clear the registers atomically.
     */
    static void atomic_clear() {
        static_assert(Policy::is_atomic && Policy::can_write,
                      "Not atomic/can't clear this reg");
    }
};
}  // namespace kerl::sys
