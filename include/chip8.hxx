#pragma once

#include <array>
#include <stack>
#include <vector>

#define CH8_FLAG_REG 0xF
#define CH8_FONT_ADDR 0x50

#define CH8_SCREEN_COLOR 0xFF
#define CH8_SCREEN_COLOR_2 0x00;
#define CH8_SCREEN_WIDTH 64
#define CH8_SCREEN_HEIGHT 32
#define CH8_SCREEN_SIZE CH8_SCREEN_WIDTH * CH8_SCREEN_HEIGHT

#define CH8_PC_START 0x200
#define CH8_PC_STEP 2

#define CH8_MEMORY_SIZE 0x1000
typedef std::array<uint8_t, CH8_MEMORY_SIZE> Memory;
typedef std::array<uint8_t, 16> RegArray;
typedef std::array<uint8_t, CH8_SCREEN_SIZE> Framebuffer;
typedef std::array<bool, 16> Keyboard;

class Chip8 {
private:
    uint16_t index_register;
    RegArray registers;
    Memory memory;
    std::stack<uint16_t> stack;

    uint8_t delay_timer;
    uint8_t sound_timer;

    Framebuffer framebuffer;

    size_t pc;
    void increment_pc();

    Keyboard keyboard;
    
    uint16_t fetch_instruction();
    void decode_instruction(uint16_t& instruction);

    void clear_fb();
public:
    static uint8_t sdl_scancode_to_key(uint16_t scancode);

    Chip8();
    ~Chip8();

    void load(std::vector<uint8_t>& program);

    void* get_fb();
    bool fb_modified;

    void set_key(uint8_t key, bool status);

    bool step();
    void run();
    void pause();
};