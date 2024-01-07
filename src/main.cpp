#include <iostream>
#include <string>
#include <fstream>
#include <cstddef>
#include <bitset>
#include <array>
#include <algorithm>
#include <cassert>
#include <SFML/Graphics.hpp>
#include "emulator.hpp"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Provide a path to the binary file.\n";
        return -1;
    }

    const std::string bin_path {argv[1]};

    Memory memory;
    memory.loadProgram(bin_path);

    Emulator emulator {&memory};
    emulator.run();

    return 0;
}
