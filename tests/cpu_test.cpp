#include "cpu.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>

namespace {

constexpr std::uint32_t kDataAddress {100};
constexpr std::uint32_t kX0 {0};
constexpr std::uint32_t kX1 {1};
constexpr std::uint32_t kX5 {5};
constexpr std::uint32_t kStartPc {0};
constexpr std::uint32_t kPcAfterOneStep {4};
constexpr std::uint32_t kIllegalOpcode {0x00000000};

constexpr std::uint32_t kAddiX1X0Imm1 {0x00100093};
constexpr std::uint32_t kAddiX1X0Imm8 {0x00800093};
constexpr std::uint32_t kAddiX1X0Imm10 {0x00A00093};
constexpr std::uint32_t kAddiX1X0ImmMinus1 {0xFFF00093};
constexpr std::uint32_t kAddiX1X0ImmMinus5 {0xFFB00093};
constexpr std::uint32_t kAddiX1X0ImmMinus16 {0xFF000093};
constexpr std::uint32_t kAddiX2X0Imm1 {0x00100113};
constexpr std::uint32_t kAddiX2X0Imm4 {0x00400113};
constexpr std::uint32_t kAddiX2X0Imm6 {0x00600113};
constexpr std::uint32_t kAddiX2X0Imm33 {0x02100113};
constexpr std::uint32_t kAddiX2X0ImmMinus1 {0xFFF00113};

class CpuTest : public ::testing::Test {
protected:
    Memory m_memory {};
    Cpu m_cpu {m_memory};

    void runInstruction(const std::uint32_t instruction) {
        m_memory.write32(m_cpu.pc(), instruction);
        m_cpu.step();
    }
};

TEST_F(CpuTest, IllegalOpcodeThrows) {
    m_memory.write32(m_cpu.pc(), kIllegalOpcode);

    EXPECT_THROW(m_cpu.step(), std::invalid_argument);
}

class CpuLoadTest : public CpuTest {};

constexpr std::uint32_t kLbX1Offset100X0 {0x06400083};
constexpr std::uint32_t kLbuX1Offset100X0 {0x06404083};
constexpr std::uint32_t kLhX1Offset100X0 {0x06401083};
constexpr std::uint32_t kLhuX1Offset100X0 {0x06405083};
constexpr std::uint32_t kLwX1Offset100X0 {0x06402083};
constexpr std::uint32_t kLwX0Offset100X0 {0x06402003};
constexpr std::uint32_t kLwX1OffsetMinus4X0 {0xFFC02083};
constexpr std::uint32_t kLoadWithIllegalFunct3 {0x06407083};

TEST_F(CpuLoadTest, LbSignExtendsByte) {
    m_memory.write32(m_cpu.pc(), kLbX1Offset100X0);
    m_memory.write8(kDataAddress, 0xFF);

    m_cpu.step();

    EXPECT_EQ(m_cpu.pc(), kPcAfterOneStep);
    EXPECT_EQ(m_cpu.registers().read(kX1), 0xFFFFFFFF);
}

TEST_F(CpuLoadTest, LbuZeroExtendsByte) {
    m_memory.write32(m_cpu.pc(), kLbuX1Offset100X0);
    m_memory.write8(kDataAddress, 0xFF);

    m_cpu.step();

    EXPECT_EQ(m_cpu.pc(), kPcAfterOneStep);
    EXPECT_EQ(m_cpu.registers().read(kX1), 0x000000FF);
}

TEST_F(CpuLoadTest, LhSignExtendsHalfWord) {
    m_memory.write32(m_cpu.pc(), kLhX1Offset100X0);
    m_memory.write16(kDataAddress, 0x8000);

    m_cpu.step();

    EXPECT_EQ(m_cpu.pc(), kPcAfterOneStep);
    EXPECT_EQ(m_cpu.registers().read(kX1), 0xFFFF8000);
}

TEST_F(CpuLoadTest, LhuZeroExtendsHalfWord) {
    m_memory.write32(m_cpu.pc(), kLhuX1Offset100X0);
    m_memory.write16(kDataAddress, 0x8000);

    m_cpu.step();

    EXPECT_EQ(m_cpu.pc(), kPcAfterOneStep);
    EXPECT_EQ(m_cpu.registers().read(kX1), 0x00008000);
}

TEST_F(CpuLoadTest, LwLoadsWord) {
    m_memory.write32(m_cpu.pc(), kLwX1Offset100X0);
    m_memory.write32(kDataAddress, 0xDEADBEEF);

    m_cpu.step();

    EXPECT_EQ(m_cpu.pc(), kPcAfterOneStep);
    EXPECT_EQ(m_cpu.registers().read(kX1), 0xDEADBEEF);
}

TEST_F(CpuLoadTest, LbKeepsPositiveByte) {
    m_memory.write32(m_cpu.pc(), kLbX1Offset100X0);
    m_memory.write8(kDataAddress, 0x7F);

    m_cpu.step();

    EXPECT_EQ(m_cpu.registers().read(kX1), 0x0000007F);
}

TEST_F(CpuLoadTest, LoadIntoX0IsIgnored) {
    m_memory.write32(m_cpu.pc(), kLwX0Offset100X0);
    m_memory.write32(kDataAddress, 0xDEADBEEF);

    m_cpu.step();

    EXPECT_EQ(m_cpu.pc(), kPcAfterOneStep);
    EXPECT_EQ(m_cpu.registers().read(kX0), 0);
}

TEST_F(CpuLoadTest, LoadOutOfRangeThrowsAndChangesNothing) {
    runInstruction(kAddiX1X0Imm1);
    m_memory.write32(m_cpu.pc(), kLwX1OffsetMinus4X0);

    EXPECT_THROW(m_cpu.step(), std::out_of_range);

    EXPECT_EQ(m_cpu.pc(), kPcAfterOneStep);
    EXPECT_EQ(m_cpu.registers().read(kX1), 1);
}

TEST_F(CpuLoadTest, IllegalFunct3Throws) {
    m_memory.write32(m_cpu.pc(), kLoadWithIllegalFunct3);

    EXPECT_THROW(m_cpu.step(), std::invalid_argument);
}

class CpuOpImmTest : public CpuTest {};

constexpr std::uint32_t kAddiX5X0Imm10 {0x00A00293};
constexpr std::uint32_t kAddiX5X0ImmMinus10 {0xFF600293};
constexpr std::uint32_t kAddiX5X1Imm1 {0x00108293};
constexpr std::uint32_t kSlliX5X1Imm1 {0x00109293};
constexpr std::uint32_t kSlliX5X1Imm31 {0x01F09293};
constexpr std::uint32_t kSlliWithIllegalFunct7 {0x20109293};
constexpr std::uint32_t kSltiX5X1ImmMinus1 {0xFFF0A293};
constexpr std::uint32_t kSltiX5X1Imm1 {0x0010A293};
constexpr std::uint32_t kSltiuX5X1ImmMinus1 {0xFFF0B293};
constexpr std::uint32_t kSltiuX5X1Imm1 {0x0010B293};
constexpr std::uint32_t kSltiuX5X1Imm2 {0x0020B293};
constexpr std::uint32_t kSrliX5X1Imm4 {0x0040D293};
constexpr std::uint32_t kSraiX5X1Imm4 {0x4040D293};
constexpr std::uint32_t kSrliWithIllegalFunct7 {0x0240D293};
constexpr std::uint32_t kXoriX5X1Imm6 {0x0060C293};
constexpr std::uint32_t kXoriX5X1ImmMinus1 {0xFFF0C293};
constexpr std::uint32_t kOriX5X1Imm6 {0x0060E293};
constexpr std::uint32_t kOriX5X1ImmMinus16 {0xFF00E293};
constexpr std::uint32_t kAndiX5X1Imm6 {0x0060F293};
constexpr std::uint32_t kAndiX5X1ImmMinus1 {0xFFF0F293};

TEST_F(CpuOpImmTest, AddiImmediatePositive) {
    runInstruction(kAddiX5X0Imm10);

    EXPECT_EQ(m_cpu.registers().read(kX5), 10);
}

TEST_F(CpuOpImmTest, AddiImmediateNegative) {
    runInstruction(kAddiX5X0ImmMinus10);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0xFFFFFFF6);
}

TEST_F(CpuOpImmTest, AddiWrapsAroundOnOverflow) {
    runInstruction(kAddiX1X0ImmMinus1);
    runInstruction(kAddiX5X1Imm1);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0);
}

TEST_F(CpuOpImmTest, SlliShiftsLeft) {
    runInstruction(kAddiX1X0Imm8);
    runInstruction(kSlliX5X1Imm1);

    EXPECT_EQ(m_cpu.registers().read(kX5), 16);
}

TEST_F(CpuOpImmTest, SlliDropsBitsShiftedOutOfTheTop) {
    runInstruction(kAddiX1X0ImmMinus1);
    runInstruction(kSlliX5X1Imm31);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0x80000000);
}

TEST_F(CpuOpImmTest, SlliWithIllegalFunct7ThrowsAndChangesNothing) {
    m_memory.write32(m_cpu.pc(), kSlliWithIllegalFunct7);

    EXPECT_THROW(m_cpu.step(), std::invalid_argument);

    EXPECT_EQ(m_cpu.pc(), kStartPc);
    EXPECT_EQ(m_cpu.registers().read(kX5), 0);
}

TEST_F(CpuOpImmTest, SltiComparesAsSigned) {
    runInstruction(kAddiX1X0Imm1);
    runInstruction(kSltiX5X1ImmMinus1);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0);
}

TEST_F(CpuOpImmTest, SltiNegativeIsLessThanPositive) {
    runInstruction(kAddiX1X0ImmMinus5);
    runInstruction(kSltiX5X1Imm1);

    EXPECT_EQ(m_cpu.registers().read(kX5), 1);
}

TEST_F(CpuOpImmTest, SltiEqualIsNotLess) {
    runInstruction(kAddiX1X0Imm1);
    runInstruction(kSltiX5X1Imm1);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0);
}

TEST_F(CpuOpImmTest, SltiuSetsWhenLess) {
    runInstruction(kAddiX1X0Imm1);
    runInstruction(kSltiuX5X1Imm2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 1);
}

TEST_F(CpuOpImmTest, SltiuSignExtendsImmediateThenComparesAsUnsigned) {
    runInstruction(kAddiX1X0Imm1);
    runInstruction(kSltiuX5X1ImmMinus1);

    EXPECT_EQ(m_cpu.registers().read(kX5), 1);
}

TEST_F(CpuOpImmTest, SltiuTreatsNegativeAsLargeUnsigned) {
    runInstruction(kAddiX1X0ImmMinus5);
    runInstruction(kSltiuX5X1Imm1);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0);
}

TEST_F(CpuOpImmTest, SltiuEqualIsNotLess) {
    runInstruction(kAddiX1X0Imm1);
    runInstruction(kSltiuX5X1Imm1);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0);
}

TEST_F(CpuOpImmTest, SrliFillsWithZeros) {
    runInstruction(kAddiX1X0ImmMinus16);
    runInstruction(kSrliX5X1Imm4);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0x0FFFFFFF);
}

TEST_F(CpuOpImmTest, SraiFillsWithSignBit) {
    runInstruction(kAddiX1X0ImmMinus16);
    runInstruction(kSraiX5X1Imm4);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0xFFFFFFFF);
}

TEST_F(CpuOpImmTest, SrliWithIllegalFunct7ThrowsAndChangesNothing) {
    m_memory.write32(m_cpu.pc(), kSrliWithIllegalFunct7);

    EXPECT_THROW(m_cpu.step(), std::invalid_argument);

    EXPECT_EQ(m_cpu.pc(), kStartPc);
    EXPECT_EQ(m_cpu.registers().read(kX5), 0);
}

TEST_F(CpuOpImmTest, XoriFlipsMatchingBits) {
    runInstruction(kAddiX1X0Imm10);
    runInstruction(kXoriX5X1Imm6);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0b1100);
}

TEST_F(CpuOpImmTest, XoriWithMinusOneIsBitwiseNot) {
    runInstruction(kAddiX1X0Imm10);
    runInstruction(kXoriX5X1ImmMinus1);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0xFFFFFFF5);
}

TEST_F(CpuOpImmTest, OriSetsBits) {
    runInstruction(kAddiX1X0Imm10);
    runInstruction(kOriX5X1Imm6);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0b1110);
}

TEST_F(CpuOpImmTest, OriSignExtendsImmediate) {
    runInstruction(kAddiX1X0Imm10);
    runInstruction(kOriX5X1ImmMinus16);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0xFFFFFFFA);
}

TEST_F(CpuOpImmTest, AndiKeepsCommonBits) {
    runInstruction(kAddiX1X0Imm10);
    runInstruction(kAndiX5X1Imm6);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0b0010);
}

TEST_F(CpuOpImmTest, AndiSignExtendsImmediate) {
    runInstruction(kAddiX1X0ImmMinus16);
    runInstruction(kAndiX5X1ImmMinus1);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0xFFFFFFF0);
}

class CpuOpTest : public CpuTest {};

constexpr std::uint32_t kAddX5X1X2 {0x002082B3};
constexpr std::uint32_t kSubX5X1X2 {0x402082B3};
constexpr std::uint32_t kSllX5X1X2 {0x002092B3};
constexpr std::uint32_t kSltX5X1X2 {0x0020A2B3};
constexpr std::uint32_t kSltuX5X1X2 {0x0020B2B3};
constexpr std::uint32_t kXorX5X1X2 {0x0020C2B3};
constexpr std::uint32_t kSrlX5X1X2 {0x0020D2B3};
constexpr std::uint32_t kSraX5X1X2 {0x4020D2B3};
constexpr std::uint32_t kOrX5X1X2 {0x0020E2B3};
constexpr std::uint32_t kAndX5X1X2 {0x0020F2B3};
constexpr std::uint32_t kMulX5X1X2 {0x022082B3};
constexpr std::uint32_t kXorWithArithmeticFunct7 {0x4020C2B3};

TEST_F(CpuOpTest, AddAddsRegisters) {
    runInstruction(kAddiX1X0Imm10);
    runInstruction(kAddiX2X0Imm6);
    runInstruction(kAddX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 16);
}

TEST_F(CpuOpTest, AddWrapsAroundOnOverflow) {
    runInstruction(kAddiX1X0ImmMinus1);
    runInstruction(kAddiX2X0Imm1);
    runInstruction(kAddX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0);
}

TEST_F(CpuOpTest, SubSubtractsRegisters) {
    runInstruction(kAddiX1X0Imm10);
    runInstruction(kAddiX2X0Imm6);
    runInstruction(kSubX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 4);
}

TEST_F(CpuOpTest, SubBelowZeroWrapsAround) {
    runInstruction(kAddiX1X0Imm1);
    runInstruction(kAddiX2X0Imm6);
    runInstruction(kSubX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0xFFFFFFFB);
}

TEST_F(CpuOpTest, SllShiftsLeft) {
    runInstruction(kAddiX1X0Imm8);
    runInstruction(kAddiX2X0Imm1);
    runInstruction(kSllX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 16);
}

TEST_F(CpuOpTest, SllUsesOnlyLowFiveBitsOfRs2) {
    runInstruction(kAddiX1X0Imm8);
    runInstruction(kAddiX2X0Imm33);
    runInstruction(kSllX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 16);
}

TEST_F(CpuOpTest, SltComparesAsSigned) {
    runInstruction(kAddiX1X0Imm1);
    runInstruction(kAddiX2X0ImmMinus1);
    runInstruction(kSltX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0);
}

TEST_F(CpuOpTest, SltNegativeIsLessThanPositive) {
    runInstruction(kAddiX1X0ImmMinus5);
    runInstruction(kAddiX2X0Imm1);
    runInstruction(kSltX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 1);
}

TEST_F(CpuOpTest, SltEqualIsNotLess) {
    runInstruction(kAddiX1X0Imm1);
    runInstruction(kAddiX2X0Imm1);
    runInstruction(kSltX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0);
}

TEST_F(CpuOpTest, SltuComparesAsUnsigned) {
    runInstruction(kAddiX1X0Imm1);
    runInstruction(kAddiX2X0ImmMinus1);
    runInstruction(kSltuX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 1);
}

TEST_F(CpuOpTest, SltuTreatsNegativeAsLargeUnsigned) {
    runInstruction(kAddiX1X0ImmMinus5);
    runInstruction(kAddiX2X0Imm1);
    runInstruction(kSltuX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0);
}

TEST_F(CpuOpTest, SltuEqualIsNotLess) {
    runInstruction(kAddiX1X0Imm1);
    runInstruction(kAddiX2X0Imm1);
    runInstruction(kSltuX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0);
}

TEST_F(CpuOpTest, SrlFillsWithZeros) {
    runInstruction(kAddiX1X0ImmMinus16);
    runInstruction(kAddiX2X0Imm4);
    runInstruction(kSrlX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0x0FFFFFFF);
}

TEST_F(CpuOpTest, SrlUsesOnlyLowFiveBitsOfRs2) {
    runInstruction(kAddiX1X0Imm8);
    runInstruction(kAddiX2X0Imm33);
    runInstruction(kSrlX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 4);
}

TEST_F(CpuOpTest, SraFillsWithSignBit) {
    runInstruction(kAddiX1X0ImmMinus16);
    runInstruction(kAddiX2X0Imm4);
    runInstruction(kSraX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0xFFFFFFFF);
}

TEST_F(CpuOpTest, SraUsesOnlyLowFiveBitsOfRs2) {
    runInstruction(kAddiX1X0ImmMinus16);
    runInstruction(kAddiX2X0Imm33);
    runInstruction(kSraX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0xFFFFFFF8);
}

TEST_F(CpuOpTest, XorFlipsMatchingBits) {
    runInstruction(kAddiX1X0Imm10);
    runInstruction(kAddiX2X0Imm6);
    runInstruction(kXorX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0b1100);
}

TEST_F(CpuOpTest, OrSetsBits) {
    runInstruction(kAddiX1X0Imm10);
    runInstruction(kAddiX2X0Imm6);
    runInstruction(kOrX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0b1110);
}

TEST_F(CpuOpTest, AndKeepsCommonBits) {
    runInstruction(kAddiX1X0Imm10);
    runInstruction(kAddiX2X0Imm6);
    runInstruction(kAndX5X1X2);

    EXPECT_EQ(m_cpu.registers().read(kX5), 0b0010);
}

TEST_F(CpuOpTest, MulIsNotSupportedAndChangesNothing) {
    runInstruction(kAddiX5X0Imm10);
    m_memory.write32(m_cpu.pc(), kMulX5X1X2);

    EXPECT_THROW(m_cpu.step(), std::invalid_argument);

    EXPECT_EQ(m_cpu.pc(), kPcAfterOneStep);
    EXPECT_EQ(m_cpu.registers().read(kX5), 10);
}

TEST_F(CpuOpTest, ArithmeticFunct7OnXorThrows) {
    m_memory.write32(m_cpu.pc(), kXorWithArithmeticFunct7);

    EXPECT_THROW(m_cpu.step(), std::invalid_argument);
}

class CpuLuiTest : public CpuTest {};

constexpr std::uint32_t kLuiX1Imm0x7FFFF {0x7FFFF0B7};
constexpr std::uint32_t kLuiX1Imm0x80000 {0x800000B7};
constexpr std::uint32_t kLuiX1Imm0x12345 {0x123450B7};
constexpr std::uint32_t kLuiX1Imm0x12346 {0x123460B7};
constexpr std::uint32_t kAddiX1X1Imm0x678 {0x67808093};
constexpr std::uint32_t kAddiX1X1ImmMinus1 {0xFFF08093};

TEST_F(CpuLuiTest, LuiPlacesImmediateInUpperBits) {
    runInstruction(kLuiX1Imm0x7FFFF);

    EXPECT_EQ(m_cpu.registers().read(kX1), 0x7FFFF000);
}

TEST_F(CpuLuiTest, LuiPreservesSignBit) {
    runInstruction(kLuiX1Imm0x80000);

    EXPECT_EQ(m_cpu.registers().read(kX1), 0x80000000);
}

TEST_F(CpuLuiTest, LuiOverwritesWholeRegister) {
    runInstruction(kAddiX1X0ImmMinus1);

    runInstruction(kLuiX1Imm0x12345);

    EXPECT_EQ(m_cpu.registers().read(kX1), 0x12345000);
}

TEST_F(CpuLuiTest, LuiThenAddiBuildsFullConstant) {
    runInstruction(kLuiX1Imm0x12345);
    runInstruction(kAddiX1X1Imm0x678);

    EXPECT_EQ(m_cpu.registers().read(kX1), 0x12345678);
}

TEST_F(CpuLuiTest, LuiRoundsUpWhenAddiImmediateIsNegative) {
    runInstruction(kLuiX1Imm0x12346);
    runInstruction(kAddiX1X1ImmMinus1);

    EXPECT_EQ(m_cpu.registers().read(kX1), 0x12345FFF);
}

class CpuAuipcTest : public CpuTest {};

constexpr std::uint32_t kAuipcX1Imm0x7FFFF {0x7FFFF097};
constexpr std::uint32_t kAuipcX1ImmMinus1 {0xFFFFF097};
constexpr std::uint32_t kAuipcX1Imm0 {0x00000097};

TEST_F(CpuAuipcTest, AuipcPlacesImmediateInUpperBitsPlusPC) {
    runInstruction(kLuiX1Imm0x7FFFF);

    runInstruction(kAuipcX1Imm0x7FFFF);

    EXPECT_EQ(m_cpu.registers().read(kX1), 0x7FFFF004);
}

TEST_F(CpuAuipcTest, AuipcWithNegativeImmediateGoesBackwards) {
    runInstruction(kAddiX1X0Imm1);

    runInstruction(kAuipcX1ImmMinus1);

    EXPECT_EQ(m_cpu.registers().read(kX1), 0xFFFFF004);
}

TEST_F(CpuAuipcTest, AuipcWithZeroImmediateReadsPc) {
    runInstruction(kAddiX1X0Imm1);

    runInstruction(kAuipcX1Imm0);

    EXPECT_EQ(m_cpu.registers().read(kX1), kPcAfterOneStep);
}

}
