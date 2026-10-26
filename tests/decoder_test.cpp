#include <gtest/gtest.h>

#include <cstdint>
#include <limits>

#include "decoder.h"

namespace {

constexpr std::uint32_t kAddX3X1X2 {0x002081B3};
constexpr std::uint32_t kSubX3X1X2 {0x402081B3};
constexpr std::uint32_t kAllBitsSet {0xFFFFFFFF};

constexpr std::uint32_t kAddiX1X0Ten {0x00A00093};
constexpr std::uint32_t kAddiX1X0One {0x00100093};
constexpr std::uint32_t kAddiX1X0MinusOne {0xFFF00093};
constexpr std::uint32_t kAddiX1X02047 {0x7FF00093};
constexpr std::uint32_t kAddiX1X0Minus2048 {0x80000093};

constexpr std::uint32_t kSwX2Offset8 {0x0020A423};
constexpr std::uint32_t kSwX2Offset2047 {0x7E20AFA3};
constexpr std::uint32_t kSwX2OffsetMinus4 {0xFE20AE23};
constexpr std::uint32_t kSwX2OffsetMinus2048 {0x8020A023};

constexpr std::uint32_t kBeqX1X2Offset8 {0x00208463};
constexpr std::uint32_t kBeqX1X2Offset2048 {0x002080E3};
constexpr std::uint32_t kBeqX1X2Offset4094 {0x7E208FE3};
constexpr std::uint32_t kBeqX1X2OffsetMinus4 {0xFE208EE3};
constexpr std::uint32_t kBeqX1X2OffsetMinus4096 {0x80208063};

constexpr std::uint32_t kLuiX1Upper12345 {0x123450B7};
constexpr std::uint32_t kLuiX1Upper0 {0x000000B7};
constexpr std::uint32_t kLuiX1UpperFFFFF {0xFFFFF0B7};
constexpr std::uint32_t kLuiX1Upper80000 {0x800000B7};

constexpr std::uint32_t kJalX1Offset8 {0x008000EF};
constexpr std::uint32_t kJalX1Offset2048 {0x001000EF};
constexpr std::uint32_t kJalX1Offset4096 {0x000010EF};
constexpr std::uint32_t kJalX1Offset1048574 {0x7FFFF0EF};
constexpr std::uint32_t kJalX1OffsetMinus4 {0xFFDFF0EF};
constexpr std::uint32_t kJalX1OffsetMinus1048576 {0x800000EF};

TEST(DecoderTest, DecodesAddiFields) {
    EXPECT_EQ(decoder::opcode(kAddiX1X0Ten), 0x13);
    EXPECT_EQ(decoder::rd(kAddiX1X0Ten), 1);
    EXPECT_EQ(decoder::funct3(kAddiX1X0Ten), 0);
    EXPECT_EQ(decoder::rs1(kAddiX1X0Ten), 0);
}

TEST(DecoderTest, DecodesAddFields) {
    EXPECT_EQ(decoder::opcode(kAddX3X1X2), 0x33);
    EXPECT_EQ(decoder::rd(kAddX3X1X2), 3);
    EXPECT_EQ(decoder::funct3(kAddX3X1X2), 0);
    EXPECT_EQ(decoder::rs1(kAddX3X1X2), 1);
    EXPECT_EQ(decoder::rs2(kAddX3X1X2), 2);
    EXPECT_EQ(decoder::funct7(kAddX3X1X2), 0x00);
}

TEST(DecoderTest, SubDiffersFromAddOnlyInFunct7) {
    EXPECT_EQ(decoder::funct7(kSubX3X1X2), 0x20);

    EXPECT_EQ(decoder::opcode(kSubX3X1X2), decoder::opcode(kAddX3X1X2));
    EXPECT_EQ(decoder::rd(kSubX3X1X2), decoder::rd(kAddX3X1X2));
    EXPECT_EQ(decoder::funct3(kSubX3X1X2), decoder::funct3(kAddX3X1X2));
    EXPECT_EQ(decoder::rs1(kSubX3X1X2), decoder::rs1(kAddX3X1X2));
    EXPECT_EQ(decoder::rs2(kSubX3X1X2), decoder::rs2(kAddX3X1X2));
}

TEST(DecoderTest, AllBitsSetGivesMaximumOfEachField) {
    EXPECT_EQ(decoder::opcode(kAllBitsSet), 0x7F);
    EXPECT_EQ(decoder::rd(kAllBitsSet), 31);
    EXPECT_EQ(decoder::funct3(kAllBitsSet), 7);
    EXPECT_EQ(decoder::rs1(kAllBitsSet), 31);
    EXPECT_EQ(decoder::rs2(kAllBitsSet), 31);
    EXPECT_EQ(decoder::funct7(kAllBitsSet), 0x7F);
}

TEST(DecoderTest, ImmIDecodesPositiveValues) {
    EXPECT_EQ(decoder::immI(kAddiX1X0One), 1);
    EXPECT_EQ(decoder::immI(kAddiX1X02047), 2047);
}

TEST(DecoderTest, ImmISignExtendsNegativeValues) {
    EXPECT_EQ(decoder::immI(kAddiX1X0MinusOne), -1);
    EXPECT_EQ(decoder::immI(kAddiX1X0Minus2048), -2048);
}

TEST(DecoderTest, ImmSDecodesPositiveValues) {
    EXPECT_EQ(decoder::immS(kSwX2Offset8), 8);
    EXPECT_EQ(decoder::immS(kSwX2Offset2047), 2047);
}

TEST(DecoderTest, ImmSSignExtendsNegativeValues) {
    EXPECT_EQ(decoder::immS(kSwX2OffsetMinus4), -4);
    EXPECT_EQ(decoder::immS(kSwX2OffsetMinus2048), -2048);
}

TEST(DecoderTest, ImmBDecodesPositiveValues) {
    EXPECT_EQ(decoder::immB(kBeqX1X2Offset8), 8);
    EXPECT_EQ(decoder::immB(kBeqX1X2Offset2048), 2048);
    EXPECT_EQ(decoder::immB(kBeqX1X2Offset4094), 4094);
}

TEST(DecoderTest, ImmBDecodesNegativeValues) {
    EXPECT_EQ(decoder::immB(kBeqX1X2OffsetMinus4), -4);
    EXPECT_EQ(decoder::immB(kBeqX1X2OffsetMinus4096), -4096);
}

TEST(DecoderTest, ImmUDecodesPositiveValues) {
    EXPECT_EQ(decoder::immU(kLuiX1Upper12345), 0x12345000);
    EXPECT_EQ(decoder::immU(kLuiX1Upper0), 0);
}

TEST(DecoderTest, ImmUDecodesNegativeValues) {
    EXPECT_EQ(decoder::immU(kLuiX1UpperFFFFF), -4096);
    EXPECT_EQ(decoder::immU(kLuiX1Upper80000), std::numeric_limits<std::int32_t>::min());
}

TEST(DecoderTest, ImmUClearsLowTwelveBits) {
    EXPECT_EQ(decoder::immU(kAllBitsSet), -4096);
}

TEST(DecoderTest, ImmJDecodesPositiveValues) {
    EXPECT_EQ(decoder::immJ(kJalX1Offset8), 8);
    EXPECT_EQ(decoder::immJ(kJalX1Offset2048), 2048);
    EXPECT_EQ(decoder::immJ(kJalX1Offset4096), 4096);
    EXPECT_EQ(decoder::immJ(kJalX1Offset1048574), 1048574);
}

TEST(DecoderTest, ImmJDecodesNegativeValues) {
    EXPECT_EQ(decoder::immJ(kJalX1OffsetMinus4), -4);
    EXPECT_EQ(decoder::immJ(kJalX1OffsetMinus1048576), -1048576);
}

TEST(DecoderTest, ImmJAllBitsSetKeepsBitZeroClear) {
    EXPECT_EQ(decoder::immJ(kAllBitsSet), -2);
}

}
