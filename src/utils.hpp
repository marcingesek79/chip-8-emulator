#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <fstream>

namespace display
{
    constexpr int PIXEL_SIZE {5};
    constexpr int WIDTH {64};
    constexpr int HEIGHT {32};
}

namespace window
{
    constexpr int WIDTH {display::WIDTH * display::PIXEL_SIZE};
    constexpr int HEIGHT {display::HEIGHT * display::PIXEL_SIZE};
    inline const std::string TITLE {"Chip-8 Emulator"};
}

namespace memory
{
    constexpr int SIZE {4096};
    constexpr int PROGRAM_OFFSET {0x200};
}

namespace font_buffer
{
    constexpr int SIZE {80};
    constexpr int OFFSET {0x50};
}


