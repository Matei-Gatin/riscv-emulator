#ifndef VM_PROJECT_MEMORY_H
#define VM_PROJECT_MEMORY_H

#include <cstddef>
#include <cstdint>
#include <vector>

class Memory {
public:
    static constexpr std::size_t s_kDefaultSize = 1024 * 1024;
    static constexpr std::uint32_t s_kBytesPerWord = 4;

    explicit Memory(const std::size_t size = s_kDefaultSize)
        : m_bytes(size) { }

    [[nodiscard]] std::size_t size() const { return m_bytes.size(); }

    [[nodiscard]] std::uint8_t read8(std::uint32_t address) const;
    void write8(std::uint32_t address, std::uint8_t value);

    [[nodiscard]] std::uint32_t read32(std::uint32_t address) const;
    void write32(std::uint32_t address, std::uint32_t value);

private:
    static constexpr std::uint32_t s_kBitsPerByte = 8;

    std::vector<std::uint8_t> m_bytes;

    void checkRange(std::uint32_t address, std::size_t length) const;
};

#endif //VM_PROJECT_MEMORY_H
