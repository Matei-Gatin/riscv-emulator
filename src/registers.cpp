#include "registers.h"

#include <format>
#include <stdexcept>

void Registers::checkValidIndex(std::uint32_t index) {
    if (index > s_registerCount) {
        throw std::out_of_range(std::format(
            "Register {} doesn't exist", index
            ));
    }
}

std::uint32_t Registers::read(const std::uint32_t index) {
    checkValidIndex(index);

    return m_registers[index];
}

void Registers::write(const std::uint32_t index, const std::uint32_t value) {
    checkValidIndex(index);

    if (index == 0) return;

    m_registers[index] = value;
}