#include "decoder.h"

namespace {

struct Field {
    std::uint32_t startBit {};
    std::uint32_t width {};
};

struct ImmediateBits {
    std::uint32_t bits {};
    std::uint32_t width {};
};

constexpr Field kOpcode {.startBit=0, .width=7};
constexpr Field kRd {.startBit=7, .width=5};
constexpr Field kFunct3 {.startBit=12, .width=3};
constexpr Field kRs1 {.startBit=15, .width=5};
constexpr Field kRs2 {.startBit=20, .width=5};
constexpr Field kFunct7 {.startBit=25, .width=7};

constexpr Field kImmI {.startBit=20, .width=12};

constexpr Field kImmS4To0 {.startBit=7, .width=5};
constexpr Field kImmS11To5 {.startBit=25, .width=7};

constexpr Field kImmB11 {.startBit=7, .width=1};
constexpr Field kImmB4To1 {.startBit=8, .width=4};
constexpr Field kImmB10To5 {.startBit=25, .width=6};
constexpr Field kImmB12 {.startBit=31, .width=1};

constexpr Field kImmJ19To12 {.startBit=12, .width=8};
constexpr Field kImmJ11 {.startBit=20, .width=1};
constexpr Field kImmJ10To1 {.startBit=21, .width=10};
constexpr Field kImmJ20 {.startBit=31, .width=1};

constexpr std::uint32_t kInstructionWidth {32};
constexpr std::uint32_t kImmSWidth {12};
constexpr std::uint32_t kImmBWidth {13};
constexpr std::uint32_t kImmUWidth {20};
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

std::uint32_t extractField(const std::uint32_t instruction, const Field field) {
    const std::uint32_t mask {(std::uint32_t {1} << field.width) - 1};
    return (instruction >> field.startBit) & mask;
}

std::uint8_t castTo8Bits(const std::uint32_t value) {
    return static_cast<std::uint8_t>(value);
}

std::int32_t signExtend(const ImmediateBits immediate) {
    const std::uint32_t signBit {std::uint32_t {1} << (immediate.width - 1)};
    const std::uint32_t signExtension {~((std::uint32_t {1} << immediate.width) - 1)};

    return (immediate.bits & signBit) != 0 ?
        static_cast<std::int32_t>(immediate.bits | signExtension) :
        static_cast<std::int32_t>(immediate.bits);
}

}

std::uint8_t decoder::opcode(const std::uint32_t instruction) {
    return castTo8Bits(extractField(instruction, kOpcode));
}

std::uint8_t decoder::rd(const std::uint32_t instruction) {
    return castTo8Bits(extractField(instruction, kRd));
}

std::uint8_t decoder::funct3(const std::uint32_t instruction) {
    return castTo8Bits(extractField(instruction, kFunct3));
}

std::uint8_t decoder::rs1(const std::uint32_t instruction) {
    return castTo8Bits(extractField(instruction, kRs1));
}

std::uint8_t decoder::rs2(const std::uint32_t instruction) {
    return castTo8Bits(extractField(instruction, kRs2));
}

std::uint8_t decoder::funct7(const std::uint32_t instruction) {
    return castTo8Bits(extractField(instruction, kFunct7));
}

std::int32_t decoder::immI(const std::uint32_t instruction) {
    const std::uint32_t imm {extractField(instruction, kImmI)};
    return signExtend({.bits=imm, .width=kImmI.width});
}

std::int32_t decoder::immS(const std::uint32_t instruction) {
    const std::uint32_t bits4To0 {extractField(instruction, kImmS4To0)};
    const std::uint32_t bits11To5 {extractField(instruction, kImmS11To5)};

    const std::uint32_t imm {(bits11To5 << kImmS11To5Position) | bits4To0};

    return signExtend({.bits=imm, .width=kImmSWidth});
}

std::int32_t decoder::immB(const std::uint32_t instruction) {
    const std::uint32_t bits4To1 {extractField(instruction, kImmB4To1)};
    const std::uint32_t bits10To5 {extractField(instruction, kImmB10To5)};
    const std::uint32_t bit11 {extractField(instruction, kImmB11)};
    const std::uint32_t bit12 {extractField(instruction, kImmB12)};

    const std::uint32_t imm {(bit12 << kImmB12Position) | (bit11 << kImmB11Position) |
                             (bits10To5 << kImmB10To5Position) | (bits4To1 << kImmB4To1Position)};

    return signExtend({.bits=imm, .width=kImmBWidth});
}

std::int32_t decoder::immU(const std::uint32_t instruction) {
    constexpr std::uint32_t mask {~((std::uint32_t {1} << (kInstructionWidth - kImmUWidth)) - 1)};
    const std::uint32_t value {instruction & mask};

    return static_cast<std::int32_t>(value);
}

std::int32_t decoder::immJ(const std::uint32_t instruction) {
    const std::uint32_t bits10To1 {extractField(instruction, kImmJ10To1)};
    const std::uint32_t bit11 {extractField(instruction, kImmJ11)};
    const std::uint32_t bits19To12 {extractField(instruction, kImmJ19To12)};
    const std::uint32_t bit20 {extractField(instruction, kImmJ20)};

    const std::uint32_t imm {(bit20 << kImmJ20Position) | (bits19To12 << kImmJ19To12Position) |
                             (bit11 << kImmJ11Position) | (bits10To1 << kImmJ10To1Position)};

    return signExtend({.bits=imm, .width=kImmJWidth});
}
