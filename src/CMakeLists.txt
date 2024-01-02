
include_directories(headers)
add_executable(chip8-emulator main.cpp)
target_compile_features(chip8-emulator PRIVATE cxx_std_17)
target_link_libraries(chip8-emulator sfml-graphics sfml-window sfml-system)
