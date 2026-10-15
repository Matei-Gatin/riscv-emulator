//
// Created by matt on 10/14/26.
//

#include "memory.h"

#include <format>
#include <stdexcept>

// Checks [address, address + length) in one go, before anything is read or written.
// Written as a subtraction so that address + length can never wrap around.
void Memory::checkRange(const std::uint32_t address, const std::size_t length) const {
    if (address >= m_bytes.size() || length > m_bytes.size() - address) {
        throw std::out_of_range(std::format(
            "memory access out of range: address 0x{:08x}, length {}, memory size {}",
            address, length, m_bytes.size()));
    }
}

std::uint8_t Memory::read8(const std::uint32_t address) const {
    checkRange(address, 1);
    return m_bytes[address];
}

void Memory::write8(const std::uint32_t address, const std::uint8_t value) {
    checkRange(address, 1);
    m_bytes[address] = value;
}

std::uint32_t Memory::read32(const std::uint32_t address) const {
    checkRange(address, kBytesPerWord);

    std::uint32_t value = 0;
    for (std::uint32_t i = 0; i < kBytesPerWord; i++) {
        value |= static_cast<std::uint32_t>(m_bytes[address + i]) << (kBitsPerByte * i);
    }
    return value;
}

void Memory::write32(const std::uint32_t address, const std::uint32_t value) {
    checkRange(address, kBytesPerWord);

    for (std::uint32_t i = 0; i < kBytesPerWord; i++) {
        m_bytes[address + i] = static_cast<std::uint8_t>(value >> (kBitsPerByte * i));
    }
}
