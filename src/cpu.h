#ifndef VM_PROJECT_CPU_H
#define VM_PROJECT_CPU_H

#include "memory.h"
#include "registers.h"

#include <cstdint>

class Cpu {
public:
    explicit Cpu(Memory& memory) : m_memory {memory} {}

    void step();

    [[nodiscard]] const Registers& registers() const {
        return m_registers;
    }

    [[nodiscard]] std::uint32_t pc() const {
        return m_pc;
    }

private:
    Registers m_registers {};

    Memory& m_memory;

    std::uint32_t m_pc {};

    void executeLoad(std::uint32_t instruction);
};

#endif //VM_PROJECT_CPU_H
