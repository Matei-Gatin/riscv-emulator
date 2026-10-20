#include "decoder.h"

namespace {

constexpr std::uint32_t kOpcodeStartBit = 0;
constexpr std::uint32_t kRdStartBit = 7;
constexpr std::uint32_t kFunct3StartBit = 12;
constexpr std::uint32_t kRs1StartBit = 15;
constexpr std::uint32_t kRs2StartBit = 20;
constexpr std::uint32_t kFunct7StartBit = 25;

constexpr std::uint32_t kOpcodeWidth = 7;
constexpr std::uint32_t kRegisterWidth = 5;
constexpr std::uint32_t kFunct3Width = 3;
constexpr std::uint32_t kFunct7Width = 7;

std::uint8_t extractField(const std::uint32_t instruction, const std::uint32_t startBit, const std::uint32_t width) {
    const std::uint32_t mask {(std::uint32_t {1} << width) - 1};
    return static_cast<std::uint8_t>((instruction >> startBit) & mask);
}

}

std::uint8_t decoder::opcode(const std::uint32_t instruction) {
    return extractField(instruction, kOpcodeStartBit, kOpcodeWidth);
}

std::uint8_t decoder::rd(const std::uint32_t instruction) {
    return extractField(instruction, kRdStartBit, kRegisterWidth);
}

std::uint8_t decoder::funct3(const std::uint32_t instruction) {
    return extractField(instruction, kFunct3StartBit, kFunct3Width);
}

std::uint8_t decoder::rs1(const std::uint32_t instruction) {
    return extractField(instruction, kRs1StartBit, kRegisterWidth);
}

std::uint8_t decoder::rs2(const std::uint32_t instruction) {
    return extractField(instruction, kRs2StartBit, kRegisterWidth);
}

std::uint8_t decoder::funct7(const std::uint32_t instruction) {
    return extractField(instruction, kFunct7StartBit, kFunct7Width);
}
