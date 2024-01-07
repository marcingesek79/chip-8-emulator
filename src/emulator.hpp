#pragma once
#include <SFML/Graphics.hpp>
#include "cpu.hpp"

class Emulator
{
private:
    sf::RenderWindow window;
    CPU cpu;

    void draw() noexcept;

public:
    explicit Emulator(Memory* memory) noexcept;
    void run() noexcept;
};
