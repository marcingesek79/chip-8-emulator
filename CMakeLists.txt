cmake_minimum_required(VERSION 3.0.0)
project(emulator VERSION 0.1.0 LANGUAGES CXX)

add_executable(emulator main.cpp)
target_compile_features(emulator PRIVATE cxx_std_17)
target_link_libraries(emulator sfml-graphics sfml-window sfml-system)
