#include <iostream>
#include <string>
#include <fstream>
#include <cstddef>
#include <bitset>
#include <array>
#include <algorithm>
#include <SFML/Graphics.hpp>

constexpr std::size_t MEMORY_SIZE {4096};
using MEMORY = std::array<uint8_t, MEMORY_SIZE>;

void loadFonts(MEMORY& mem_buffer)
{
    constexpr std::size_t FONT_BUFFER_SIZE {80};
    std::array<uint8_t, FONT_BUFFER_SIZE> font_buffer = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    constexpr int MEM_BUFFER_OFFSET {0x50};
    std::copy(std::begin(font_buffer), std::end(font_buffer), std::begin(mem_buffer) + MEM_BUFFER_OFFSET);
}

MEMORY loadProgram(const std::string& path)
{
    std::ifstream ifs {path, std::ios::binary};
    MEMORY memory_buffer;
    
    // Initialization: 0x000 to 0x1FF is reserved
    int idx {0x200};

    while (ifs.good())
    {
        memory_buffer[idx] = ifs.get();
        idx++;
    }

    loadFonts(memory_buffer);
    return memory_buffer;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Provide a path to the binary file.\n";
        return -1;
    }

    const std::string binary_file_path {argv[1]};
    std::cout << "Path: " << binary_file_path << '\n';

    const std::string window_title {"Chip-8 Emulator"};
    sf::RenderWindow window {sf::VideoMode{200, 200}, window_title};

    const MEMORY mem_buffer {loadProgram(binary_file_path)};

    for (const auto& byte : mem_buffer)
    {
        std::cout << static_cast<int>(byte) << '\n';
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear();
        window.display();
    }

    return 0;
}
