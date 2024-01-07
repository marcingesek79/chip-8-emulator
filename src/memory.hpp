#pragma once
#include "utils.hpp"

using memory_t = std::array<uint8_t, memory::SIZE>;

class Memory
{
private:
    memory_t memory_buffer{};
    
    void loadFonts() noexcept;

public:
    Memory() noexcept;
    void loadProgram(const std::string& bin_path) noexcept;
    [[nodiscard]] uint8_t getByte(int offset) const noexcept;
};
