#include "gtest/gtest.h"


#include <cstdint>
#include <stdexcept>

#include "registers.h"

namespace {

TEST(RegistersTest, ReadFromRegisterReturnsValue) {
    Registers registers {};

    registers.write(5, 10);
    EXPECT_EQ(registers.read(5), 10);
}

TEST(RegistersTest, ReadFromRegister0Returns0) {
    Registers registers {};

    registers.write(0, 10);
    EXPECT_EQ(registers.read(0), 0);
}

TEST(RegistersTest, ReadWriteRegisterAbove31Throws) {
    Registers registers {};

    EXPECT_THROW((void)registers.write(32, 10), std::out_of_range);
    EXPECT_THROW((void)registers.read(32), std::out_of_range);
}
}
