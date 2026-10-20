#ifndef VM_PROJECT_DECODER_H
#define VM_PROJECT_DECODER_H

#include <cstdint>

namespace decoder {

[[nodiscard]] std::uint8_t opcode(std::uint32_t instruction);
[[nodiscard]] std::uint8_t rd(std::uint32_t instruction);
[[nodiscard]] std::uint8_t funct3(std::uint32_t instruction);
[[nodiscard]] std::uint8_t rs1(std::uint32_t instruction);
[[nodiscard]] std::uint8_t rs2(std::uint32_t instruction);
[[nodiscard]] std::uint8_t funct7(std::uint32_t instruction);

}

#endif //VM_PROJECT_DECODER_H
