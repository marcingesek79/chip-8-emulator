
add_library(chip8-emulator-lib cpu.cpp emulator.cpp)
add_executable(chip8-emulator main.cpp cpu.cpp emulator.cpp)
target_compile_features(chip8-emulator PRIVATE cxx_std_17)
target_link_libraries(chip8-emulator sfml-graphics sfml-window sfml-system)
