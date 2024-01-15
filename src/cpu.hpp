#pragma once
#include "memory.hpp"
#include "display.hpp"
#include <iostream>
#include <stack>
#include <random>
#include <mutex>
#include <thread>
#include <chrono>

struct Instruction
{
    uint32_t first_nibble : 4;
    uint32_t second_nibble : 4;
    uint32_t third_nibble : 4;
    uint32_t fourth_nibble : 4;
    uint32_t asWord : 16;

    constexpr Instruction() noexcept :
        first_nibble{},
        second_nibble{},
        third_nibble{},
        fourth_nibble{},
        asWord{}
    {
    }

    constexpr Instruction(uint8_t first_byte, uint8_t second_byte) noexcept :
        first_nibble {static_cast<uint32_t>(first_byte & 0xF0) >> 4},
        second_nibble {static_cast<uint32_t>(first_byte & 0x0F)},
        third_nibble {static_cast<uint32_t>(second_byte & 0xF0) >> 4},
        fourth_nibble {static_cast<uint32_t>(second_byte & 0x0F)},
        asWord {static_cast<uint32_t>((first_byte << 8) | second_byte)}
    {
    }
};

using gp_regs_t = std::array<uint8_t, 16>;

class CPU
{
private:
    Memory* memory;
    uint8_t delay_timer {10};
    uint8_t sound_timer {200};
    static std::mutex delay_timer_mutex;
    static std::mutex sound_timer_mutex;
    
    void processSpriteRow(uint8_t sprite_byte, int x, int y) noexcept;
    
    // Instruction set
    void setProgramCounter() noexcept;
    void setRegister() noexcept;
    void addToRegister() noexcept;
    void setIndexRegister() noexcept;
    void drawOnDisplay() noexcept;
    void assignment() noexcept;
    void OR() noexcept;
    void AND() noexcept;
    void XOR() noexcept;
    void addWithCarry() noexcept;
    void subtract() noexcept;
    void callSubroutine() noexcept;
    void returnFromSubroutine() noexcept;
    void shiftLeft() noexcept;
    void shiftRight() noexcept;
    void jumpWithOffset() noexcept;
    void random() noexcept;
    void assignDelayTimer() noexcept;
    void setDelayTimer() noexcept;
    void setSoundTimer() noexcept;

public:
    Display display {};
    gp_regs_t gp_regs {};
    int program_counter {memory::PROGRAM_OFFSET};
    int index_reg {};
    Instruction inst;
    std::stack<int> cpu_stack {};

    explicit CPU(Memory* memory) noexcept;

    void decreaseDelayTimer() noexcept;
    void decreaseSoundTimer() noexcept;

    void fetch() noexcept;
    void decode() noexcept;
};
