#ifndef VM_PROJECT_REGISTERS_H
#define VM_PROJECT_REGISTERS_H

#include <array>
#include <cstdint>

class Registers {
public:
    static constexpr std::uint32_t s_kRegisterCount = 32;

    [[nodiscard]] std::uint32_t read(std::uint32_t index) const;
    void write(std::uint32_t index, std::uint32_t value);

private:
    static constexpr std::uint32_t s_kZeroRegister = 0;

    std::array<std::uint32_t, s_kRegisterCount> m_registers {};

    std::uint32_t m_pc {};

    static void checkIndex(std::uint32_t index);
};

#endif //VM_PROJECT_REGISTERS_H
