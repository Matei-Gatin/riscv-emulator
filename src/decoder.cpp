#include "decoder.h"

namespace {

constexpr std::uint32_t kOpcodeStartBit {0};
constexpr std::uint32_t kRdStartBit {7};
constexpr std::uint32_t kFunct3StartBit {12};
constexpr std::uint32_t kRs1StartBit {15};
constexpr std::uint32_t kRs2StartBit {20};
constexpr std::uint32_t kFunct7StartBit {25};
constexpr std::uint32_t kImmIStartBit {20};
constexpr std::uint32_t kImmS4To0StartBit {7};
constexpr std::uint32_t kImmS11To5StartBit {25};
constexpr std::uint32_t kImmB11StartBit {7};
constexpr std::uint32_t kImmB4To1StartBit {8};
constexpr std::uint32_t kImmB10To5StartBit {25};
constexpr std::uint32_t kImmB12StartBit {31};
constexpr std::uint32_t kImmJ10To1StartBit {21};
constexpr std::uint32_t kImmJ11StartBit {20};
constexpr std::uint32_t kImmJ19To12StartBit {12};
constexpr std::uint32_t kImmJ20StartBit {31};

constexpr std::uint32_t kInstructionWidth {32};
constexpr std::uint32_t kOpcodeWidth {7};
constexpr std::uint32_t kRegisterWidth {5};
constexpr std::uint32_t kFunct3Width {3};
constexpr std::uint32_t kFunct7Width {7};
constexpr std::uint32_t kSingleBitWidth {1};
constexpr std::uint32_t kImmIWidth {12};
constexpr std::uint32_t kImmS4To0Width {5};
constexpr std::uint32_t kImmS11To5Width {7};
constexpr std::uint32_t kImmSWidth {12};
constexpr std::uint32_t kImmB4To1Width {4};
constexpr std::uint32_t kImmB10To5Width {6};
constexpr std::uint32_t kImmBWidth {13};
constexpr std::uint32_t kImmUWidth {20};
constexpr std::uint32_t kImmJ10To1Width {10};
constexpr std::uint32_t kImmJ19To12Width {8};
constexpr std::uint32_t kImmJWidth {21};

constexpr std::uint32_t kImmS11To5Position {5};

constexpr std::uint32_t kImmB4To1Position {1};
constexpr std::uint32_t kImmB10To5Position {5};
constexpr std::uint32_t kImmB11Position {11};
constexpr std::uint32_t kImmB12Position {12};

constexpr std::uint32_t kImmJ10To1Position {1};
constexpr std::uint32_t kImmJ11Position {11};
constexpr std::uint32_t kImmJ19To12Position {12};
constexpr std::uint32_t kImmJ20Position {20};

std::uint32_t extractField(const std::uint32_t instruction, const std::uint32_t startBit, const std::uint32_t width) {
    const std::uint32_t mask {(std::uint32_t {1} << width) - 1};
    return (instruction >> startBit) & mask;
}

std::uint8_t castTo8Bits(const std::uint32_t value) {
    return static_cast<std::uint8_t>(value);
}

std::int32_t signExtend(const std::uint32_t value, const std::uint32_t width) {
    const std::uint32_t signBit {std::uint32_t {1} << (width - 1)};
    const std::uint32_t signExtension {~((std::uint32_t {1} << width) - 1)};

    return (value & signBit) != 0 ?
        static_cast<std::int32_t>(value | signExtension) :
        static_cast<std::int32_t>(value);
}

}

std::uint8_t decoder::opcode(const std::uint32_t instruction) {
    return castTo8Bits(extractField(instruction, kOpcodeStartBit, kOpcodeWidth));
}

std::uint8_t decoder::rd(const std::uint32_t instruction) {
    return castTo8Bits(extractField(instruction, kRdStartBit, kRegisterWidth));
}

std::uint8_t decoder::funct3(const std::uint32_t instruction) {
    return castTo8Bits(extractField(instruction, kFunct3StartBit, kFunct3Width));
}

std::uint8_t decoder::rs1(const std::uint32_t instruction) {
    return castTo8Bits(extractField(instruction, kRs1StartBit, kRegisterWidth));
}

std::uint8_t decoder::rs2(const std::uint32_t instruction) {
    return castTo8Bits(extractField(instruction, kRs2StartBit, kRegisterWidth));
}

std::uint8_t decoder::funct7(const std::uint32_t instruction) {
    return castTo8Bits(extractField(instruction, kFunct7StartBit, kFunct7Width));
}

std::int32_t decoder::immI(const std::uint32_t instruction) {
    const std::uint32_t imm {extractField(instruction, kImmIStartBit, kImmIWidth)};
    return signExtend(imm, kImmIWidth);
}

std::int32_t decoder::immS(const std::uint32_t instruction) {
    const std::uint32_t bits4To0 {extractField(instruction, kImmS4To0StartBit, kImmS4To0Width)};
    const std::uint32_t bits11To5 {extractField(instruction, kImmS11To5StartBit, kImmS11To5Width)};

    const std::uint32_t imm {(bits11To5 << kImmS11To5Position) | bits4To0};

    return signExtend(imm, kImmSWidth);
}

std::int32_t decoder::immB(const std::uint32_t instruction) {
    const std::uint32_t bits4To1 {extractField(instruction, kImmB4To1StartBit, kImmB4To1Width)};
    const std::uint32_t bits10To5 {extractField(instruction, kImmB10To5StartBit, kImmB10To5Width)};
    const std::uint32_t bit11 {extractField(instruction, kImmB11StartBit, kSingleBitWidth)};
    const std::uint32_t bit12 {extractField(instruction, kImmB12StartBit, kSingleBitWidth)};

    const std::uint32_t imm {(bit12 << kImmB12Position) | (bit11 << kImmB11Position) |
                             (bits10To5 << kImmB10To5Position) | (bits4To1 << kImmB4To1Position)};

    return signExtend(imm, kImmBWidth);
}

std::int32_t decoder::immU(const std::uint32_t instruction) {
    constexpr std::uint32_t mask {~((std::uint32_t {1} << (kInstructionWidth - kImmUWidth)) - 1)};
    const std::uint32_t value {instruction & mask};

    return static_cast<std::int32_t>(value);
}

std::int32_t decoder::immJ(const std::uint32_t instruction) {
    const std::uint32_t bits10To1 {extractField(instruction, kImmJ10To1StartBit, kImmJ10To1Width)};
    const std::uint32_t bit11 {extractField(instruction, kImmJ11StartBit, kSingleBitWidth)};
    const std::uint32_t bits19To12 {extractField(instruction, kImmJ19To12StartBit, kImmJ19To12Width)};
    const std::uint32_t bit20 {extractField(instruction, kImmJ20StartBit, kSingleBitWidth)};

    const std::uint32_t imm {(bit20 << kImmJ20Position) | (bits19To12 << kImmJ19To12Position) |
                             (bit11 << kImmJ11Position) | (bits10To1 << kImmJ10To1Position)};

    return signExtend(imm, kImmJWidth);
}
