#include "cpu.hpp"

void CPU::processSpriteRow(uint8_t sprite_byte, int x, int y) noexcept
{
    // Iterating bit by bit from the left to the right
    for (int j{0}; j < 8; ++j)
    {
        // Getting current bit
        const uint8_t mask = 1 << (7 - j); 
        const uint8_t current_pixel = (sprite_byte & mask) >> (7 - j);
        
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
        if (x >= display::WIDTH)
            break;
    }
}

void CPU::setProgramCounter(const Instruction& inst) noexcept
{
    program_counter = (inst.second_nibble << 8) | (inst.third_nibble << 4) | inst.fourth_nibble;
}

void CPU::setRegister(const Instruction& inst) noexcept
{
    gp_regs[inst.second_nibble] = (inst.third_nibble << 4) | inst.fourth_nibble;
}

void CPU::addToRegister(const Instruction& inst) noexcept
{
    gp_regs[inst.second_nibble] += (inst.third_nibble << 4) | inst.fourth_nibble;
}

void CPU::setIndexRegister(const Instruction& inst) noexcept
{
    index_reg = (inst.second_nibble << 8) | (inst.third_nibble << 4) | inst.fourth_nibble;
}

void CPU::drawOnDisplay(const Instruction& inst) noexcept
{
    int x = gp_regs[inst.second_nibble] & (display::WIDTH - 1);
    int y = gp_regs[inst.third_nibble] & (display::HEIGHT - 1);
    gp_regs[0xF] = 0;

    for (int i{0}; i < inst.fourth_nibble; ++i)
    {
        uint8_t sprite_byte = memory->getByte(index_reg + i);
        processSpriteRow(sprite_byte, x, y);

        y++;
        if (y >= display::HEIGHT)
            break;
    }
}

void CPU::assignment(const Instruction& inst) noexcept
{
    gp_regs[inst.second_nibble] = gp_regs[inst.third_nibble];
}

void CPU::OR(const Instruction& inst) noexcept
{
    gp_regs[inst.second_nibble] |= gp_regs[inst.third_nibble];
}

void CPU::AND(const Instruction& inst) noexcept
{
    gp_regs[inst.second_nibble] &= gp_regs[inst.third_nibble];
}

void CPU::XOR(const Instruction& inst) noexcept
{
    gp_regs[inst.second_nibble] ^= gp_regs[inst.third_nibble];
}

void CPU::AddWithCarry(const Instruction& inst) noexcept
{
    const int result {gp_regs[inst.second_nibble] + gp_regs[inst.third_nibble]};
    gp_regs[0xF] = result > 0xFF;
    gp_regs[inst.second_nibble] += gp_regs[inst.third_nibble];
}

void CPU::Subtract(const Instruction& inst) noexcept
{
    const int result {gp_regs[inst.second_nibble] - gp_regs[inst.third_nibble]};
    gp_regs[0xF] = !(result < 0);
    gp_regs[inst.second_nibble] -= gp_regs[inst.third_nibble];
}

CPU::CPU(Memory* memory) noexcept
    : memory{memory}
{
}

Instruction CPU::fetch() noexcept
{
    const uint8_t first_byte {memory->getByte(program_counter)};
    const uint8_t second_byte {memory->getByte(program_counter + 1)};
    program_counter += 2;

    return {first_byte, second_byte};
}

void CPU::decode(const Instruction& inst) noexcept
{
    if (inst.asWord == 0x00E0)
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
