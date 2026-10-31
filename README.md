# RISC-V Emulator

A user-mode emulator for the RISC-V RV32I instruction set, written in C++20.

## What works so far

- Memory: byte-addressable, little-endian, with 8, 16 and 32-bit reads and writes. Every access
  is bounds-checked and throws if it goes outside memory.
- Registers: the 32 registers `x0` to `x31`. `x0` always reads 0.
- Decoder: splits a 32-bit instruction into its fields (opcode, rd, rs1, rs2, funct3, funct7)
  and rebuilds the sign-extended immediates for all six formats (R, I, S, B, U, J).
- CPU: fetches the instruction at the pc, decodes it and executes it. The load instructions
  (`lb`, `lh`, `lw`, `lbu`, `lhu`) work. Unknown instructions throw an error.

## Plan

1. CPU state and decoding (done)
2. Execute all 40 base instructions (in progress)
3. Load ELF programs compiled with Clang
4. System calls (`read`, `write`, `exit`) so a C "hello world" runs
5. Stretch goals: traps, paging or performance work

## Build and test

Needs CMake 4.1 and a C++20 compiler. GoogleTest is downloaded by CMake on the first configure.

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Every push runs the tests on GitHub Actions, once normally and once with AddressSanitizer and
UBSan, and checks the code with clang-tidy.

## Layout

- `src/` the emulator (`memory`, `registers`, `decoder`, `cpu`)
- `tests/` GoogleTest tests, one file per source file
