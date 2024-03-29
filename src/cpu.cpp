#include "cpu.hpp"

std::mutex CPU::delay_timer_mutex;
std::mutex CPU::sound_timer_mutex;

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

void CPU::decreaseDelayTimer() noexcept
{   
    while (1)
    {
        std::lock_guard<std::mutex> guard {CPU::delay_timer_mutex};
        delay_timer -= (delay_timer > 0);
        std::cout << "\nDelay: " << static_cast<int>(delay_timer) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(timer::TIMER_FREQ_IN_MILLISECONDS));
    }
}

void CPU::decreaseSoundTimer() noexcept
{
    while (1)
    {
        std::lock_guard<std::mutex> guard {CPU::sound_timer_mutex};

        if (sound_timer > 0)
        {
            std::cout << '\a';
            sound_timer -= 1;
        }

        std::cout << "\nSound: " << static_cast<int>(sound_timer) << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(timer::TIMER_FREQ_IN_MILLISECONDS));
    }
}

void CPU::setProgramCounter() noexcept
{
    program_counter = (inst.second_nibble << 8) | (inst.third_nibble << 4) | inst.fourth_nibble;
}

void CPU::setRegister() noexcept
{
    gp_regs[inst.second_nibble] = (inst.third_nibble << 4) | inst.fourth_nibble;
}

void CPU::addToRegister() noexcept
{
    gp_regs[inst.second_nibble] += (inst.third_nibble << 4) | inst.fourth_nibble;
}

void CPU::setIndexRegister() noexcept
{
    index_reg = (inst.second_nibble << 8) | (inst.third_nibble << 4) | inst.fourth_nibble;
}

void CPU::drawOnDisplay() noexcept
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

void CPU::assignment() noexcept
{
    gp_regs[inst.second_nibble] = gp_regs[inst.third_nibble];
}

void CPU::OR() noexcept
{
    gp_regs[inst.second_nibble] |= gp_regs[inst.third_nibble];
}

void CPU::AND() noexcept
{
    gp_regs[inst.second_nibble] &= gp_regs[inst.third_nibble];
}

void CPU::XOR() noexcept
{
    gp_regs[inst.second_nibble] ^= gp_regs[inst.third_nibble];
}

void CPU::addWithCarry() noexcept
{
    const int result {gp_regs[inst.second_nibble] + gp_regs[inst.third_nibble]};
    gp_regs[0xF] = result > 0xFF;
    gp_regs[inst.second_nibble] += gp_regs[inst.third_nibble];
}

void CPU::subtract() noexcept
{
    const int result {gp_regs[inst.second_nibble] - gp_regs[inst.third_nibble]};
    gp_regs[0xF] = !(result < 0);
    gp_regs[inst.second_nibble] -= gp_regs[inst.third_nibble];
}

void CPU::callSubroutine() noexcept
{
    cpu_stack.push(program_counter);
    program_counter = (inst.second_nibble << 8) | (inst.third_nibble << 4) | inst.fourth_nibble;
}

void CPU::returnFromSubroutine() noexcept
{
    program_counter = cpu_stack.top();
    cpu_stack.pop();
}

void CPU::shiftLeft() noexcept
{
    gp_regs[inst.second_nibble] = gp_regs[inst.third_nibble];
    int msb {(gp_regs[inst.second_nibble] & 0x80) >> 7};
    gp_regs[inst.second_nibble] <<= 1;
    gp_regs[0xF] = msb ? 1 : 0;
}

void CPU::shiftRight() noexcept
{
    gp_regs[inst.second_nibble] = gp_regs[inst.third_nibble];
    int lsb {gp_regs[inst.second_nibble] & 0x1};
    gp_regs[inst.second_nibble] >>= 1;
    gp_regs[0xF] = lsb ? 1 : 0;
}

void CPU::jumpWithOffset() noexcept
{
    program_counter = (inst.second_nibble << 8) | (inst.third_nibble << 4) | inst.fourth_nibble;
    program_counter += gp_regs[0];
}

void CPU::random() noexcept
{
    std::mt19937 mt {std::random_device{}()};
    std::uniform_int_distribution generate {0, 0xFF}; // generate uint8_t
    uint8_t generated_number {static_cast<uint8_t>(generate(mt))};
    generated_number &= ((inst.third_nibble << 4) | inst.fourth_nibble);
    gp_regs[inst.second_nibble] = generated_number;
}

void CPU::assignDelayTimer() noexcept
{
    std::lock_guard<std::mutex> guard {CPU::delay_timer_mutex};
    gp_regs[inst.second_nibble] = delay_timer;
}

void CPU::setDelayTimer() noexcept
{
    std::lock_guard<std::mutex> guard {CPU::delay_timer_mutex};
    delay_timer = gp_regs[inst.second_nibble];
}

void CPU::setSoundTimer() noexcept
{
    std::lock_guard<std::mutex> guard {CPU::sound_timer_mutex};
    sound_timer = gp_regs[inst.second_nibble];
}

CPU::CPU(Memory* memory) noexcept
    : memory{memory}
{
}

void CPU::fetch() noexcept
{
    const uint8_t first_byte {memory->getByte(program_counter)};
    const uint8_t second_byte {memory->getByte(program_counter + 1)};
    program_counter += 2;

    inst = {first_byte, second_byte};
}

void CPU::decode() noexcept
{
    if (inst.asWord == 0x00E0)
    {
        display.clear();
    }
    else if (inst.asWord == 0x00EE)
    {
        returnFromSubroutine();
    }
    else if (inst.first_nibble == 0x1)
    {
        setProgramCounter();
    }
    else if (inst.first_nibble == 0x2)
    {
        callSubroutine();
    }
    else if (inst.first_nibble == 0x6)
    {
        setRegister();
    }
    else if (inst.first_nibble == 0x7)
    {
        addToRegister();
    }
    else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0x0)
    {
        assignment();
    }
    else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0x1)
    {
        OR();
    }
    else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0x2)
    {
        AND();
    }
    else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0x3)
    {
        XOR();
    }
    else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0x4)
    {
        addWithCarry();
    }
    else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0x5)
    {
        subtract();
    }
    else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0x6)
    {
        shiftRight();
    }
    else if (inst.first_nibble == 0x8 && inst.fourth_nibble == 0xE)
    {
        shiftLeft();
    }
    else if (inst.first_nibble == 0xA)
    {
        setIndexRegister();
    }
    else if (inst.first_nibble == 0xB)
    {
        jumpWithOffset();
    }
    else if (inst.first_nibble == 0xC)
    {
        random();
    }
    else if (inst.first_nibble == 0xD)
    {
        drawOnDisplay();
    }
    else if (inst.first_nibble == 0xF && 
            inst.third_nibble == 0x0 && 
            inst.fourth_nibble == 0x7)
    {
        assignDelayTimer();
    }
    else if (inst.first_nibble == 0xF &&
            inst.third_nibble == 0x1 &&
            inst.fourth_nibble == 0x5)
    {
        setDelayTimer();
    }
    else if (inst.first_nibble == 0xF &&
            inst.third_nibble == 0x1 &&
            inst.fourth_nibble == 0x8)
    {
        setSoundTimer();
    }
}
