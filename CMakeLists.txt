cmake_minimum_required(VERSION 3.14)
project(emulator VERSION 0.1.0 LANGUAGES CXX)

add_executable(emulator emulator.cpp)
target_compile_features(emulator PRIVATE cxx_std_17)
target_link_libraries(emulator sfml-graphics sfml-window sfml-system)
