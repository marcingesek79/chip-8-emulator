#pragma once
#include "utils.hpp"

enum class Pixel
{
    off = 0,
    on = 1
};

using display_t = std::array<std::array<Pixel, display::WIDTH>, display::HEIGHT>;

class Display
{
private:
    display_t display_buffer {};

public:
    [[nodiscard]] constexpr Pixel get(int x, int y) const noexcept
    {
        return display_buffer[y][x];
    }

    constexpr void set(int x, int y, Pixel val) noexcept
    {
        display_buffer[y][x] = val;
    }

    constexpr void clear() noexcept
    {
        for (int y{0}; y < display::HEIGHT; ++y)
        {
            for (int x{0}; x < display::WIDTH; ++x)
            {
                set(x, y, Pixel::off);
            }
        }
    }
};
