
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(unit_tests tests.cpp)

target_include_directories(unit_tests PUBLIC ../src)

target_link_libraries(
  unit_tests
  GTest::gtest_main
  gmock_main
  chip8-emulator-lib
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
