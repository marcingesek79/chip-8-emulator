#include "emulator.hpp"

void Emulator::draw() noexcept
{        
    for (int y{0}; y < display::HEIGHT; ++y)
    {
        for (int x{0}; x < display::WIDTH; ++x)
        {
            if (cpu.display.get(x, y) == Pixel::on)
            {
                sf::RectangleShape rect{{display::PIXEL_SIZE, display::PIXEL_SIZE}};
                rect.setFillColor(sf::Color::White);

                constexpr auto scale {static_cast<float>(display::PIXEL_SIZE)};
                rect.setPosition({x * scale, y * scale});
                
                window.draw(rect);
            }
        }
    }
}

Emulator::Emulator(Memory* memory) noexcept :
    window {sf::VideoMode{window::WIDTH, window::HEIGHT}, window::TITLE},
    cpu {memory}
{   
}

void Emulator::run() noexcept
{
    while (window.isOpen())
    {
        window.clear();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        cpu.fetch();
        cpu.decode();

        draw();

        window.display();
    }    
}
