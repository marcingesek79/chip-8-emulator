#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/headers/display.hpp"
#include "../src/headers/memory.hpp"
#include "../src/headers/cpu.hpp"

class MockDisplay : public Display
{
public:
    MOCK_METHOD(Pixel, get, (int x, int y), ());
    MOCK_METHOD(void, set, (int x, int y, Pixel val), ());
    MOCK_METHOD(void, clear, (), ());
};

class MockMemory : public Memory
{
public:
    MOCK_METHOD(void, loadFonts, (), ());
    MOCK_METHOD(void, loadProgram, (const std::string& bin_path), ());
    MOCK_METHOD(uint8_t, getByte, (int offset), ());
};

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

TEST(CPU, Instruction)
{
    uint8_t first_byte {0xAB};
    uint8_t second_byte {0xCD};
    Instruction inst {first_byte, second_byte};

    EXPECT_EQ(inst.first_nibble, 0xA);
    EXPECT_EQ(inst.second_nibble, 0xB);
    EXPECT_EQ(inst.third_nibble, 0xC);
    EXPECT_EQ(inst.fourth_nibble, 0xD);
}

TEST(CPU, setProgramCounter)
{
    MockMemory memory;
    CPU cpu {memory};

    Instruction inst1{0x1B, 0xCD};
    cpu.decode(inst1);
    EXPECT_EQ(cpu.getProgramCounter(), 0xBCD);

    Instruction inst2{0x12, 0x00};
    cpu.decode(inst2);
    EXPECT_EQ(cpu.getProgramCounter(), 0x200);
}

TEST(CPU, setRegister)
{
    MockMemory memory;
    CPU cpu {memory};

    Instruction inst1{0x61, 0x11};
    cpu.decode(inst1);
    EXPECT_EQ(cpu.getGPRegs()[0x1], 0x11);

    Instruction inst2{0x6A, 0x22};
    cpu.decode(inst2);
    EXPECT_EQ(cpu.getGPRegs()[0xA], 0x22);

    Instruction inst3{0x65, 0x33};
    cpu.decode(inst3);
    EXPECT_EQ(cpu.getGPRegs()[0x5], 0x33);
}

TEST(CPU, addToRegister)
{
    MockMemory memory;
    CPU cpu {memory};

    Instruction inst1{0x71, 0x11};
    cpu.decode(inst1);
    EXPECT_EQ(cpu.getGPRegs()[0x1], 0x11);

    Instruction inst2{0x71, 0x22};
    cpu.decode(inst2);
    EXPECT_EQ(cpu.getGPRegs()[0x1], 0x33);
}

TEST(CPU, setIndexRegister)
{
    MockMemory memory;
    CPU cpu {memory};

    Instruction inst1{0xA1, 0x23};
    cpu.decode(inst1);
    EXPECT_EQ(cpu.getIndexReg(), 0x123);
}