#pragma once
#include <SFML/Graphics.hpp>
#include "cpu.hpp"

class Emulator
{
private:
    sf::RenderWindow window{};
    CPU cpu;

    void draw() noexcept
    {
        const Display& display {cpu.getDisplay()};
        
        for (int y{0}; y < DISPLAY_HEIGHT; ++y)
        {
            for (int x{0}; x < DISPLAY_WIDTH; ++x)
            {
                if (display.get(x, y) == Pixel::on)
                {
                    sf::RectangleShape rect{{PIXEL_SIZE, PIXEL_SIZE}};
                    rect.setFillColor(sf::Color::White);

                    constexpr float scale {static_cast<float>(PIXEL_SIZE)};
                    rect.setPosition({x * scale, y * scale});
                    
                    window.draw(rect);
                }
            }
        }
    }

public:
    Emulator(Memory& memory) noexcept
        :   window {sf::VideoMode{WINDOW_WIDTH, WINDOW_HEIGHT}, WINDOW_TITLE},
            cpu{memory}
    {
    }

    void run() noexcept
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

            Instruction inst {cpu.fetch()};
            cpu.decode(inst);

            draw();

            window.display();
        }
    }
};

