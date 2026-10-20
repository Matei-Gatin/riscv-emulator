#include <gtest/gtest.h>

#include <cstdint>

#include "decoder.h"

namespace {

constexpr std::uint32_t kAddiX1X0Ten = 0x00A00093;
constexpr std::uint32_t kAddX3X1X2 = 0x002081B3;
constexpr std::uint32_t kSubX3X1X2 = 0x402081B3;
constexpr std::uint32_t kAllBitsSet = 0xFFFFFFFF;

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

}
