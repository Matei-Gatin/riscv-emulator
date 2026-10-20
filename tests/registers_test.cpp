#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>

#include "registers.h"

namespace {

constexpr std::uint32_t kLastRegister {Registers::s_kRegisterCount - 1};

TEST(RegistersTest, StartsZeroed) {
    const Registers registers {};

    for (std::uint32_t index {0}; index < Registers::s_kRegisterCount; index++) {
        EXPECT_EQ(registers.read(index), 0) << "at register x" << index;
    }
}

TEST(RegistersTest, WriteThenReadReturnsSameValue) {
    Registers registers {};

    registers.write(5, 0xDEADBEEF);

    EXPECT_EQ(registers.read(5), 0xDEADBEEF);
}

TEST(RegistersTest, WriteToX0IsIgnored) {
    Registers registers {};

    registers.write(0, 10);

    EXPECT_EQ(registers.read(0), 0);
}

TEST(RegistersTest, LastRegisterIsAccessible) {
    Registers registers {};

    registers.write(kLastRegister, 10);

    EXPECT_EQ(registers.read(kLastRegister), 10);
}

TEST(RegistersTest, IndexPastLastRegisterThrows) {
    Registers registers {};

    EXPECT_THROW(registers.write(Registers::s_kRegisterCount, 10), std::out_of_range);
    EXPECT_THROW((void)registers.read(Registers::s_kRegisterCount), std::out_of_range);
}

}
