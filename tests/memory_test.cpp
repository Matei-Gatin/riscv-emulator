#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>

#include "memory.h"

namespace {

constexpr std::size_t kSmallSize = 16;

TEST(MemoryTest, StartsZeroed) {
    const Memory memory{kSmallSize};

    for (std::uint32_t address = 0; address < kSmallSize; address++) {
        EXPECT_EQ(memory.read8(address), 0) << "at address " << address;
    }
}

TEST(MemoryTest, Write8ThenRead8ReturnsSameByte) {
    Memory memory{};

    memory.write8(0x1000, 0xAB);

    EXPECT_EQ(memory.read8(0x1000), 0xAB);
}

TEST(MemoryTest, Write32StoresLowestByteFirst) {
    Memory memory{};

    memory.write32(0x100, 0x12345678);

    EXPECT_EQ(memory.read8(0x100), 0x78);
    EXPECT_EQ(memory.read8(0x101), 0x56);
    EXPECT_EQ(memory.read8(0x102), 0x34);
    EXPECT_EQ(memory.read8(0x103), 0x12);
}

TEST(MemoryTest, Read32TreatsLowestAddressAsLowestByte) {
    Memory memory{};
    memory.write8(0x100, 0x78);
    memory.write8(0x101, 0x56);
    memory.write8(0x102, 0x34);
    memory.write8(0x103, 0x12);

    EXPECT_EQ(memory.read32(0x100), 0x12345678u);
}

TEST(MemoryTest, Write32ThenRead32RoundTrips) {
    Memory memory{};

    memory.write32(0x1002, 100000000);

    EXPECT_EQ(memory.read32(0x1002), 100000000u);
}

TEST(MemoryTest, LastWholeWordIsAccessible) {
    Memory memory{kSmallSize};
    const auto lastWord = static_cast<std::uint32_t>(kSmallSize - Memory::s_kBytesPerWord);

    memory.write32(lastWord, 0xDEADBEEF);

    EXPECT_EQ(memory.read32(lastWord), 0xDEADBEEFu);
}

TEST(MemoryTest, Read8PastEndThrows) {
    const Memory memory{kSmallSize};

    EXPECT_THROW((void)memory.read8(kSmallSize), std::out_of_range);
}

TEST(MemoryTest, Read32CrossingEndThrows) {
    const Memory memory{kSmallSize};

    EXPECT_THROW((void)memory.read32(kSmallSize - 2), std::out_of_range);
}

TEST(MemoryTest, FailedWrite32ChangesNothing) {
    Memory memory{kSmallSize};

    EXPECT_THROW(memory.write32(kSmallSize - 2, 0xAABBCCDD), std::out_of_range);

    EXPECT_EQ(memory.read8(kSmallSize - 2), 0);
    EXPECT_EQ(memory.read8(kSmallSize - 1), 0);
}

TEST(MemoryTest, AddressNearTopOfAddressSpaceThrows) {
    const Memory memory{kSmallSize};

    EXPECT_THROW((void)memory.read32(0xFFFFFFFE), std::out_of_range);
}

}
