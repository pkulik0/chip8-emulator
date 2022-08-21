#pragma once

#include <array>
#include <stack>
#include <vector>

#define CH8_SCREEN_COLOR 0xFF
#define CH8_SCREEN_COLOR_2 0x00;

#define CH8_SCREEN_WIDTH 64
#define CH8_SCREEN_HEIGHT 32
#define CH8_SCREEN_SIZE CH8_SCREEN_WIDTH * CH8_SCREEN_HEIGHT


#define CH8_PC_STEP 2
#define CH8_REG_SIZE 16
#define CH8_MEMORY_SIZE 0x1000
#define CH8_PROGRAM_SIZE CH8_MEMORY_SIZE - CH8_PROGRAM_ADDR

#define CH8_FONT_ADDR 0x50
#define CH8_PROGRAM_ADDR 0x200

class Chip8 {
private:
    uint16_t index_register;
    std::array<uint8_t, CH8_REG_SIZE> registers;
    std::array<uint8_t, CH8_MEMORY_SIZE> memory;
    std::stack<uint16_t> stack;

    uint8_t delay_timer;
    uint8_t sound_timer;

    std::array<uint8_t, CH8_SCREEN_SIZE> framebuffer;
    bool fb_modified;

    size_t pc;
    void increment_pc();

    std::array<bool, 16> keyboard;
    
    uint16_t fetch_instruction();
    void decode_instruction(const uint16_t& instruction);

    void clear_fb();
public:
    Chip8();

    void load(const std::vector<uint8_t>& program);
    void set_key(const uint8_t& scancode, const bool& status);
    void* get_fb();
    bool step();
};