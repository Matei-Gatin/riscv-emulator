//
// Created by matt on 10/14/26.
//

#ifndef VM_PROJECT_MEMORY_H
#define VM_PROJECT_MEMORY_H

#include <cstddef>
#include <cstdint>
#include <vector>

// Guest RAM: a fixed block of bytes. Guest address N is index N.
// Multi-byte values are stored little-endian: byte i of a value lives at address + i.
// Any access outside the block throws std::out_of_range and changes nothing.
class Memory {
public:
    static constexpr std::size_t kDefaultSize = 1024 * 1024; // 1 MiB
    static constexpr std::uint32_t kBytesPerWord = 4;

    explicit Memory(std::size_t size = kDefaultSize)
        : m_bytes(size) { }

    [[nodiscard]] std::size_t size() const { return m_bytes.size(); }

    [[nodiscard]] std::uint8_t read8(std::uint32_t address) const;
    void write8(std::uint32_t address, std::uint8_t value);

    [[nodiscard]] std::uint32_t read32(std::uint32_t address) const;
    void write32(std::uint32_t address, std::uint32_t value);

private:
    static constexpr std::uint32_t kBitsPerByte = 8;

    std::vector<std::uint8_t> m_bytes;

    void checkRange(std::uint32_t address, std::size_t length) const;
};

#endif //VM_PROJECT_MEMORY_H
