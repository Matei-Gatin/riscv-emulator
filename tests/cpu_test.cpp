#include "cpu.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace {

constexpr std::uint32_t kDataAddress {100};
constexpr std::uint32_t kX1 {1};
constexpr std::uint32_t kPcAfterOneStep {4};

constexpr std::uint32_t kLbX1Offset100X0 {0x06400083};
constexpr std::uint32_t kLbuX1Offset100X0 {0x06404083};
constexpr std::uint32_t kLhX1Offset100X0 {0x06401083};
constexpr std::uint32_t kLhuX1Offset100X0 {0x06405083};
constexpr std::uint32_t kLwX1Offset100X0 {0x06402083};

class CpuTest : public ::testing::Test {
protected:
    Memory m_memory {};
    Cpu m_cpu {m_memory};
};

TEST_F(CpuTest, LbSignExtendsByte) {
    m_memory.write32(m_cpu.getPc(), kLbX1Offset100X0);
    m_memory.write8(kDataAddress, 0xFF);

    m_cpu.step();

    EXPECT_EQ(m_cpu.getPc(), kPcAfterOneStep);
    EXPECT_EQ(m_cpu.getRegister().read(kX1), 0xFFFFFFFF);
}

TEST_F(CpuTest, LbuZeroExtendsByte) {
    m_memory.write32(m_cpu.getPc(), kLbuX1Offset100X0);
    m_memory.write8(kDataAddress, 0xFF);

    m_cpu.step();

    EXPECT_EQ(m_cpu.getPc(), kPcAfterOneStep);
    EXPECT_EQ(m_cpu.getRegister().read(kX1), 0x000000FF);
}

TEST_F(CpuTest, LhSignExtendsHalfWord) {
    m_memory.write32(m_cpu.getPc(), kLhX1Offset100X0);
    m_memory.write16(kDataAddress, 0x8000);

    m_cpu.step();

    EXPECT_EQ(m_cpu.getPc(), kPcAfterOneStep);
    EXPECT_EQ(m_cpu.getRegister().read(kX1), 0xFFFF8000);
}

TEST_F(CpuTest, LhuZeroExtendsHalfWord) {
    m_memory.write32(m_cpu.getPc(), kLhuX1Offset100X0);
    m_memory.write16(kDataAddress, 0x8000);

    m_cpu.step();

    EXPECT_EQ(m_cpu.getPc(), kPcAfterOneStep);
    EXPECT_EQ(m_cpu.getRegister().read(kX1), 0x00008000);
}

TEST_F(CpuTest, LwLoadsWord) {
    m_memory.write32(m_cpu.getPc(), kLwX1Offset100X0);
    m_memory.write32(kDataAddress, 0xDEADBEEF);

    m_cpu.step();

    EXPECT_EQ(m_cpu.getPc(), kPcAfterOneStep);
    EXPECT_EQ(m_cpu.getRegister().read(kX1), 0xDEADBEEF);
}

}
