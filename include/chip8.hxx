#pragma once

#include <array>
#include <stack>
#include <mutex>
#include <vector>
#include <unordered_map>

#define CH8_SCREEN_COLOR_ON 0xFF
#define CH8_SCREEN_COLOR_OFF 0x00;

#define CH8_SCREEN_WIDTH 64
#define CH8_SCREEN_HEIGHT 32
#define CH8_SCREEN_SIZE CH8_SCREEN_WIDTH * CH8_SCREEN_HEIGHT

#define CH8_CPU_FREQUENCY 500
#define CH8_TIMER_FREQUENCY 60
#define CH8_CYCLE_TIME 1'000'000'000 / CH8_CPU_FREQUENCY

#define CH8_PC_STEP 2

#define CH8_REG_SIZE 16
#define CH8_FLAG CH8_REG_SIZE-1

#define CH8_MEMORY_SIZE 0x1000
#define CH8_PROGRAM_SIZE CH8_MEMORY_SIZE - CH8_PROGRAM_ADDR

#define CH8_FONT_ADDR 0x50
#define CH8_PROGRAM_ADDR 0x200

class Chip8 {
private:
    uint16_t index_register;
    std::array<uint8_t, CH8_REG_SIZE> reg;
    std::array<uint8_t, CH8_MEMORY_SIZE> memory;
    std::stack<uint16_t> stack;

    uint8_t delay_timer;
    uint8_t sound_timer;
    size_t timer_count;

    void handle_timers();

    std::array<uint8_t, CH8_SCREEN_SIZE> framebuffer;
    std::mutex fb_lock;
    bool fb_modified;

    void clear_fb();

    size_t pc;
    void increment_pc();

    std::array<bool, 16> keyboard;
    static const std::unordered_map<uint8_t, uint8_t> keymap;
    
    void start_main_loop();
    uint16_t fetch_instruction();
    void decode_instruction(const uint16_t ins);
    
    void draw_sprite(const uint8_t x, const uint8_t y, const uint8_t height, const uint16_t sprite_addr);

    static constexpr std::array<uint8_t, 80> font {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
public:
    Chip8();

    bool is_beeping;

    void load(const std::vector<uint8_t>& program);
    void set_key(const uint8_t scancode, const bool status);
    void* get_fb();
    bool step();
    std::thread run();
};