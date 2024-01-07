#pragma once
#include "memory.hpp"
#include "display.hpp"

struct Instruction
{
    uint8_t first_nibble;
    uint8_t second_nibble;
    uint8_t third_nibble;
    uint8_t fourth_nibble;
    uint32_t word;

    Instruction(uint8_t first_byte, uint8_t second_byte) noexcept
    {
        first_nibble = (first_byte & 0xF0) >> 4;
        second_nibble = (first_byte & 0x0F);
        third_nibble = (second_byte & 0xF0) >> 4;
        fourth_nibble = (second_byte & 0x0F);

        word = (first_byte << 8) | second_byte;
    }
};

using gp_regs_t = std::array<uint8_t, 16>;

class CPU
{
private:
    Memory* memory;
    
    void processSpriteRow(uint8_t sprite_byte, int x, int y) noexcept;

    // Instruction set
    void setProgramCounter(const Instruction& inst) noexcept;
    void setRegister(const Instruction& inst) noexcept;
    void addToRegister(const Instruction& inst) noexcept;
    void setIndexRegister(const Instruction& inst) noexcept;
    void drawOnDisplay(const Instruction& inst) noexcept;
    void assignment(const Instruction& inst) noexcept;
    void OR(const Instruction& inst) noexcept;
    void AND(const Instruction& inst) noexcept;
    void XOR(const Instruction& inst) noexcept;
    void AddWithCarry(const Instruction& inst) noexcept;
    void Subtract(const Instruction& inst) noexcept;

public:
    Display display {};
    gp_regs_t gp_regs {};
    int program_counter {};
    int index_reg {};

    explicit CPU(Memory* memory) noexcept;
    [[nodiscard]] Instruction fetch() noexcept;

    void decode(const Instruction& inst) noexcept;
};
