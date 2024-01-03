#pragma once
#include "memory.hpp"
#include "display.hpp"

struct Instruction
{
    uint8_t first_nibble;
    uint8_t second_nibble;
    uint8_t third_nibble;
    uint8_t fourth_nibble;

    Instruction(uint8_t first_byte, uint8_t second_byte)
    {
        first_nibble = (first_byte & 0xF0) >> 4;
        second_nibble = (first_byte & 0x0F);
        third_nibble = (second_byte & 0xF0) >> 4;
        fourth_nibble = (second_byte & 0x0F);
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

    Instruction fetch() noexcept
    {
        uint8_t first_byte = memory.getByte(program_counter);
        uint8_t second_byte = memory.getByte(program_counter + 1);
        program_counter += 2;

        return {first_byte, second_byte};
    }

    void decode(const Instruction& inst) noexcept
    {
        switch (inst.first_nibble)
        {
            case 0:
                display.clear();
                break;
            case 1:
                setProgramCounter(inst);
                break;
            case 6:
                setRegister(inst);
                break;
            case 7:
                addToRegister( inst);
                break;
            case 0xA:
                setIndexRegister(inst);
                break;
            case 0xD:
                drawOnDisplay(inst);
                break;
        }
    }

    
};