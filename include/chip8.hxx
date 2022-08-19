#pragma once

#include <array>
#include <stack>
#include <vector>

#define CH8_MEMORY_SIZE 4096
#define CH8_REG_NUM 16

#define CH8_SCREEN_COLOR 0xFF
#define CH8_SCREEN_COLOR_2 0x00;
#define CH8_SCREEN_WIDTH 64
#define CH8_SCREEN_HEIGHT 32
#define CH8_SCREEN_SIZE CH8_SCREEN_WIDTH * CH8_SCREEN_HEIGHT

#define CH8_PC_START 0x200
#define CH8_PC_STEP 2

#define CH8_CPU_FREQ 500

typedef std::array<uint8_t, CH8_MEMORY_SIZE> Memory;
typedef std::array<uint8_t, CH8_REG_NUM> RegArray;
typedef std::array<uint8_t, CH8_SCREEN_SIZE> Framebuffer;

class Chip8 {
private:
    uint16_t index_register;
    RegArray registers;
    Memory memory;
    std::stack<uint16_t> stack;

    uint8_t delay_timer;
    uint8_t sound_timer;

    size_t pc;
    void reset_pc();
    void increment_pc();
    
    uint16_t fetch_instruction();
    void decode_instruction(uint16_t& instruction);

    void clear_fb();

public:
    Framebuffer framebuffer;

    Chip8();
    ~Chip8();

    void load(std::vector<uint8_t>& program);

    void step();
    void run();
    void pause();
};