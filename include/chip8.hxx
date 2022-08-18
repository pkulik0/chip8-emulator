#pragma once

#include <array>
#include <vector>
#include <stack>
#include <bitset>

#define CH8_MEMORY_SIZE 4096
#define CH8_REG_NUM 16
#define CH8_SCREEN_SIZE 64*32

#define CH8_PC_START 0x200
#define CH8_PC_STEP 2

#define CH8_CPU_FREQ 500

typedef std::vector<uint8_t> ByteCode;
typedef std::bitset<CH8_SCREEN_SIZE> Framebuffer;

class Chip8 {
private:
    uint16_t index_register;
    std::array<u_int8_t, CH8_REG_NUM> registers;
    std::array<uint8_t, CH8_MEMORY_SIZE> memory;
    std::stack<u_int16_t> stack;

    uint8_t delay_timer;
    uint8_t sound_timer;

    Framebuffer framebuffer;

    size_t pc;
    void reset_pc();
    void increment_pc();

public:
    Chip8();
    ~Chip8();

    void load(ByteCode& bytecode);

    void step();
    void run();
    void pause();
};