#include "registers.h"

#include <format>
#include <stdexcept>

void Registers::checkIndex(const std::uint32_t index) {
    if (index >= s_kRegisterCount) {
        throw std::out_of_range(std::format(
            "register index out of range: {}, register count {}", index, s_kRegisterCount));
    }
}

std::uint32_t Registers::read(const std::uint32_t index) const {
    checkIndex(index);
    return m_registers[index];
}

void Registers::write(const std::uint32_t index, const std::uint32_t value) {
    checkIndex(index);

    if (index == s_kZeroRegister) {
        return;
    }

    m_registers[index] = value;
}
