#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <fstream>

constexpr int PIXEL_SIZE {5};

constexpr int DISPLAY_WIDTH {64};
constexpr int DISPLAY_HEIGHT {32};

constexpr int WINDOW_WIDTH {DISPLAY_WIDTH * PIXEL_SIZE};
constexpr int WINDOW_HEIGHT {DISPLAY_HEIGHT * PIXEL_SIZE};
inline const std::string WINDOW_TITLE {"Chip-8 Emulator"};

constexpr int MEMORY_SIZE {4096};
constexpr int PROGRAM_OFFSET {0x200};

constexpr int FONT_BUFFER_SIZE {80};
constexpr int FONT_BUFFER_OFFSET {0x50};
