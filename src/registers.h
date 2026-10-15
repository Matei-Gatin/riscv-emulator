//
// Created by matt on 10/15/26.
//

#ifndef VM_PROJECT_REGISTERS_H
#define VM_PROJECT_REGISTERS_H

#include <array>
#include <cstdint>

class Registers {
public:
    explicit Registers() = default;

    std::uint32_t read(std::uint32_t index);
    void write(std::uint32_t index, std::uint32_t value);

private:
    static constexpr std::size_t s_registerCount = 31; // we start from zero

    std::array<std::uint32_t, s_registerCount> m_registers {};

    std::uint32_t m_pc {};

    static void checkValidIndex(std::uint32_t index);
};

#endif //VM_PROJECT_REGISTERS_H
