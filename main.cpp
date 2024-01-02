#include <iostream>
#include <string>
#include <fstream>
#include <cstddef>
#include <bitset>
#include <SFML/Graphics.hpp>

std::vector<std::byte> loadProgram(const std::string& path)
{
    std::ifstream ifs {path, std::ios::binary};
    std::vector<std::byte> program;

    while (ifs.good())
    {
        program.push_back(static_cast<std::byte>(ifs.get()));
    }

    return program;
}

std::ostream& operator<<(std::ostream& os, std::byte b)
{
    return os << std::bitset<8>(std::to_integer<int>(b));
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

    const auto program {loadProgram(binary_file_path)};
    for (const auto& byte : program)
    {
        std::cout << byte << '\n';
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
