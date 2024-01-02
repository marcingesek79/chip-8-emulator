#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/headers/display.hpp"
#include "../src/headers/memory.hpp"

TEST(Display, GetAndSet)
{
    Display display;
    
    EXPECT_EQ(display.get(0, 0), Pixel::off);

    display.set(2, 5, Pixel::on);
    EXPECT_EQ(display.get(2, 5), Pixel::on);

    display.set(2, 5, Pixel::off);
    EXPECT_EQ(display.get(2, 5), Pixel::off);
}

TEST(Display, Clear)
{
    Display display;

    display.set(2, 5, Pixel::on);
    display.set(30, 30, Pixel::on);
    display.set(25, 15, Pixel::on);

    display.clear();

    EXPECT_EQ(display.get(2, 5), Pixel::off);
    EXPECT_EQ(display.get(30, 30), Pixel::off);
    EXPECT_EQ(display.get(25, 15), Pixel::off);
}

TEST(Memory, LoadFonts)
{
    Memory memory;

    EXPECT_EQ(memory.getByte(FONT_BUFFER_OFFSET), 0xF0);
    EXPECT_EQ(memory.getByte(FONT_BUFFER_OFFSET + 1), 0x90);
    EXPECT_EQ(memory.getByte(FONT_BUFFER_OFFSET + 2), 0x90);
    EXPECT_EQ(memory.getByte(FONT_BUFFER_OFFSET + 3), 0x90);
    EXPECT_EQ(memory.getByte(FONT_BUFFER_OFFSET + 4), 0xF0);
}