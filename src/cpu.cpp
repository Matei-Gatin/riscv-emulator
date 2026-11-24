#include "cpu.h"

#include <cstdint>
#include <format>
#include <stdexcept>

#include "decoder.h"

namespace {

constexpr std::uint32_t kInstructionSize {4};
constexpr std::uint8_t kFunct7Logical {0x00};
constexpr std::uint8_t kFunct7Arithmetic {0x20};
constexpr std::uint32_t kShiftAmountMask {0x1F};
constexpr std::uint8_t kFunct3Width {3};

constexpr std::uint16_t opInstructionKey(const std::uint8_t funct7, const std::uint8_t funct3) {
    return static_cast<std::uint16_t>((static_cast<std::uint16_t>(funct7) << kFunct3Width) |
                                      funct3);
}

enum class Opcode : std::uint8_t {
    Load = 0x03,
    MiscMem = 0x0F,
    OpImm = 0x13,
    Auipc = 0x17,
    Store = 0x23,
    Op = 0x33,
    Lui = 0x37,
    Branch = 0x63,
    Jalr = 0x67,
    Jal = 0x6F,
    System = 0x73
};

enum class LoadFunct3 : std::uint8_t {
    Lb = 0x0,
    Lh = 0x1,
    Lw = 0x2,
    Lbu = 0x4,
    Lhu = 0x5
};

enum class OpImmFunct3 : std::uint8_t {
    Addi = 0b000,
    Slli = 0b001,
    Slti = 0b010,
    Sltiu = 0b011,
    Xori = 0b100,
    SrliOrSrai = 0b101,
    Ori = 0b110,
    Andi = 0b111,
};

enum class OpInstruction : std::uint16_t {
    Add = opInstructionKey(kFunct7Logical, 0x0),
    Sll = opInstructionKey(kFunct7Logical, 0x1),
    Slt = opInstructionKey(kFunct7Logical, 0x2),
    Sltu = opInstructionKey(kFunct7Logical, 0x3),
    Xor = opInstructionKey(kFunct7Logical, 0x4),
    Srl = opInstructionKey(kFunct7Logical, 0x5),
    Or = opInstructionKey(kFunct7Logical, 0x6),
    And = opInstructionKey(kFunct7Logical, 0x7),

    Sub = opInstructionKey(kFunct7Arithmetic, 0x0),
    Sra = opInstructionKey(kFunct7Arithmetic, 0x5),
};

}

void Cpu::executeLoad(const std::uint32_t instruction) {
    const std::uint32_t baseAddress {m_registers.read(decoder::rs1(instruction))};
    const auto offset {static_cast<std::uint32_t>(decoder::immI(instruction))};
    const std::uint32_t address {baseAddress + offset};

    std::uint32_t value {};

    switch (static_cast<LoadFunct3>(decoder::funct3(instruction))) {
        case LoadFunct3::Lb:
            value = static_cast<std::uint32_t>(static_cast<std::int8_t>(m_memory.read8(address)));
            break;
        case LoadFunct3::Lh:
            value = static_cast<std::uint32_t>(static_cast<std::int16_t>(m_memory.read16(address)));
            break;
        case LoadFunct3::Lw:
            value = m_memory.read32(address);
            break;
        case LoadFunct3::Lbu:
            value = m_memory.read8(address);
            break;
        case LoadFunct3::Lhu:
            value = m_memory.read16(address);
            break;
        default:
            throw std::invalid_argument(
                    std::format("illegal load instruction: 0x{:08x}", instruction));
    }

    m_registers.write(decoder::rd(instruction), value);
}

void Cpu::executeOpImm(const std::uint32_t instruction) {
    const std::uint32_t rs1 {m_registers.read(decoder::rs1(instruction))};
    const auto imm {static_cast<std::uint32_t>(decoder::immI(instruction))};
    const std::uint8_t shamt {decoder::rs2(instruction)};
    const std::uint8_t funct7 {decoder::funct7(instruction)};

    std::uint32_t value {};

    switch (static_cast<OpImmFunct3>(decoder::funct3(instruction))) {
        case OpImmFunct3::Addi:
            value = rs1 + imm;
            break;
        case OpImmFunct3::Slli:
            if (funct7 != kFunct7Logical) {
                throw std::invalid_argument(
                        std::format("illegal slli instruction: 0x{:08x}", instruction));
            }
            value = rs1 << shamt;
            break;
        case OpImmFunct3::Slti:
            value = static_cast<std::int32_t>(rs1) < static_cast<std::int32_t>(imm) ? 1 : 0;
            break;
        case OpImmFunct3::Sltiu:
            value = rs1 < imm ? 1 : 0;
            break;
        case OpImmFunct3::Xori:
            value = rs1 ^ imm;
            break;
        case OpImmFunct3::SrliOrSrai:
            if (funct7 == kFunct7Logical) {
                value = rs1 >> shamt;
            } else if (funct7 == kFunct7Arithmetic) {
                value = static_cast<std::uint32_t>(static_cast<std::int32_t>(rs1) >> shamt);
            } else {
                throw std::invalid_argument(
                        std::format("illegal srli/srai instruction: 0x{:08x}", instruction));
            }
            break;
        case OpImmFunct3::Ori:
            value = rs1 | imm;
            break;
        case OpImmFunct3::Andi:
            value = rs1 & imm;
            break;
    }

    m_registers.write(decoder::rd(instruction), value);
}

void Cpu::executeOp(const std::uint32_t instruction) {
    const OpInstruction opInstruction {
            opInstructionKey(decoder::funct7(instruction), decoder::funct3(instruction))};
    const std::uint32_t rs1 {m_registers.read(decoder::rs1(instruction))};
    const std::uint32_t rs2 {m_registers.read(decoder::rs2(instruction))};

    std::uint32_t value {};

    switch (opInstruction) {
        case OpInstruction::Add:
            value = rs1 + rs2;
            break;
        case OpInstruction::Sll:
            value = rs1 << (rs2 & kShiftAmountMask);
            break;
        case OpInstruction::Slt:
            value = static_cast<std::int32_t>(rs1) < static_cast<std::int32_t>(rs2) ? 1 : 0;
            break;
        case OpInstruction::Sltu:
            value = rs1 < rs2 ? 1 : 0;
            break;
        case OpInstruction::Xor:
            value = rs1 ^ rs2;
            break;
        case OpInstruction::Srl:
            value = rs1 >> (rs2 & kShiftAmountMask);
            break;
        case OpInstruction::Or:
            value = rs1 | rs2;
            break;
        case OpInstruction::And:
            value = rs1 & rs2;
            break;
        case OpInstruction::Sub:
            value = rs1 - rs2;
            break;
        case OpInstruction::Sra:
            value = static_cast<std::uint32_t>(static_cast<std::int32_t>(rs1) >>
                                               (rs2 & kShiftAmountMask));
            break;
        default:
            throw std::invalid_argument(
                    std::format("illegal op instruction: 0x{:08x}", instruction));
    }

    m_registers.write(decoder::rd(instruction), value);
}

void Cpu::executeLui(const std::uint32_t instruction) {
    const std::uint32_t imm {static_cast<std::uint32_t>(decoder::immU(instruction))};

    m_registers.write(decoder::rd(instruction), imm);
}

void Cpu::executeAuipc(const std::uint32_t instruction) {
    const std::uint32_t imm {static_cast<std::uint32_t>(decoder::immU(instruction))};

    m_registers.write(decoder::rd(instruction), imm + m_pc);
}

void Cpu::step() {
    const std::uint32_t instruction {m_memory.read32(m_pc)};

    switch (static_cast<Opcode>(decoder::opcode(instruction))) {
        case Opcode::Load:
            executeLoad(instruction);
            break;
        case Opcode::MiscMem:
            break;
        case Opcode::OpImm:
            executeOpImm(instruction);
            break;
        case Opcode::Auipc:
            executeAuipc(instruction);
            break;
        case Opcode::Store:
            break;
        case Opcode::Op:
            executeOp(instruction);
            break;
        case Opcode::Lui:
            executeLui(instruction);
            break;
        case Opcode::Branch:
            break;
        case Opcode::Jalr:
            break;
        case Opcode::Jal:
            break;
        case Opcode::System:
            break;
        default:
            throw std::invalid_argument(std::format(
                    "unknown opcode in instruction 0x{:08x} at pc 0x{:08x}", instruction, m_pc));
    }

    m_pc += kInstructionSize;
}
