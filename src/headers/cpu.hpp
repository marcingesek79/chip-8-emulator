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

    Instruction(uint8_t first_byte, uint8_t second_byte)
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
    Memory& memory;
    Display display{};
    gp_regs_t gp_regs{};
    int program_counter{};
    int index_reg{};

    void setProgramCounter(const Instruction& inst) noexcept
    {
        int value {(inst.second_nibble << 8) | (inst.third_nibble << 4) | inst.fourth_nibble};
        program_counter = value;
    }

    void setRegister(const Instruction& inst) noexcept
    {
        gp_regs[inst.second_nibble] = (inst.third_nibble << 4) | inst.fourth_nibble;
    }

    void addToRegister(const Instruction& inst) noexcept
    {
        gp_regs[inst.second_nibble] += (inst.third_nibble << 4) | inst.fourth_nibble;
    }

    void setIndexRegister(const Instruction& inst) noexcept
    {
        index_reg = (inst.second_nibble << 8) | (inst.third_nibble << 4) | inst.fourth_nibble;
    }

    void processSpriteRow(uint8_t sprite_byte, int x, int y) noexcept
    {
        // Iterating bit by bit from the left to the right
        for (int j{0}; j < 8; ++j)
        {
            // Getting current bit
            uint8_t mask = 1 << (7 - j); 
            uint8_t current_pixel = (sprite_byte & mask) >> (7 - j);
            
            if (current_pixel)
            {
                if (display.get(x, y) == Pixel::on)
                {
                    display.set(x, y, Pixel::off);
                    gp_regs[0xF] = 1;
                }
                else
                {
                    display.set(x, y, Pixel::on);
                }
            }

            x++;
            if (x >= DISPLAY_WIDTH)
                break;
        }
    }

    void drawOnDisplay(const Instruction& inst) noexcept
    {
        int x = gp_regs[inst.second_nibble] & (DISPLAY_WIDTH - 1);
        int y = gp_regs[inst.third_nibble] & (DISPLAY_HEIGHT - 1);
        gp_regs[0xF] = 0;

        for (int i{0}; i < inst.fourth_nibble; ++i)
        {
            uint8_t sprite_byte = memory.getByte(index_reg + i);
            processSpriteRow(sprite_byte, x, y);

            y++;
            if (y >= DISPLAY_HEIGHT)
                break;
        }
    }

    void assignment(const Instruction& inst) noexcept
    {
        gp_regs[inst.second_nibble] = gp_regs[inst.third_nibble];
    }

    void OR(const Instruction& inst) noexcept
    {
        gp_regs[inst.second_nibble] |= gp_regs[inst.third_nibble];
    }

    void AND(const Instruction& inst) noexcept
    {
        gp_regs[inst.second_nibble] &= gp_regs[inst.third_nibble];
    }

    void XOR(const Instruction& inst) noexcept
    {
        gp_regs[inst.second_nibble] ^= gp_regs[inst.third_nibble];
    }

    void AddWithCarry(const Instruction& inst) noexcept
    {
        int result = gp_regs[inst.second_nibble] + gp_regs[inst.third_nibble];
        gp_regs[0xF] = result > 0xFF;
        gp_regs[inst.second_nibble] += gp_regs[inst.third_nibble];
    }

    void Subtract(const Instruction& inst) noexcept
    {
        int result = gp_regs[inst.second_nibble] - gp_regs[inst.third_nibble];
        gp_regs[0xF] = !(result < 0);
        gp_regs[inst.second_nibble] -= gp_regs[inst.third_nibble];
    }

public:
    CPU(Memory& memory) noexcept
    :   memory{memory}
    {
    }

    const Display& getDisplay() const noexcept
    {
        return display;
    }

    int getProgramCounter() const noexcept
    {
        return program_counter;
    }

    int getIndexReg() const noexcept
    {
        return index_reg;
    }

    gp_regs_t getGPRegs() const noexcept
    {
        return gp_regs;
    }

    gp_regs_t& getGPRegs() noexcept
    {
        return gp_regs;
    }

    Instruction fetch() noexcept
    {
        uint8_t first_byte = memory.getByte(program_counter);
        uint8_t second_byte = memory.getByte(program_counter + 1);
        program_counter += 2;

        return {first_byte, second_byte};
    }

    void decode(const Instruction& inst) noexcept
    {
        if (inst.word == 0x00E0)
        {
            display.clear();
        }
        else if (inst.first_nibble == 0x1)
        {
            setProgramCounter(inst);
        }
        else if (inst.first_nibble == 0x6)
        {
            setRegister(inst);
        }
        else if (inst.first_nibble == 0x7)
        {
            addToRegister(inst);
        }
        else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0x0)
        {
            assignment(inst);
        }
        else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0x1)
        {
            OR(inst);
        }
        else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0x2)
        {
            AND(inst);
        }
        else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0x3)
        {
            XOR(inst);
        }
        else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0x4)
        {
            AddWithCarry(inst);
        }
        else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0x5)
        {
            Subtract(inst);
        }
        else if (inst.first_nibble == 0xA)
        {
            setIndexRegister(inst);
        }
        else if (inst.first_nibble == 0xD)
        {
            drawOnDisplay(inst);
        }
    }
};