#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <fstream>

constexpr int DISPLAY_WIDTH {64};
constexpr int DISPLAY_HEIGHT {32};

constexpr int WINDOW_WIDTH {200};
constexpr int WINDOW_HEIGHT {200};
inline const std::string WINDOW_TITLE {"Chip-8 Emulator"};

constexpr int MEMORY_SIZE {4096};
constexpr int PROGRAM_OFFSET {0x200};

constexpr int FONT_BUFFER_SIZE {80};
constexpr int FONT_BUFFER_OFFSET {0x50};
