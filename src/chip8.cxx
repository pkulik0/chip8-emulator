#include <iostream>
#include <cstdio>

#include <thread>
#include <chrono>

#include <SDL2/SDL_keyboard.h>

#include "../include/chip8.hxx"
#include "../include/font.hxx"

Chip8::Chip8() : index_register{}, registers{}, memory{}, stack{}, delay_timer{}, sound_timer{}, framebuffer{}, pc{}, keyboard{}, fb_modified{true} {
    srand(time(0));
    for(size_t i = 0; i < font.size(); i++) {
        memory[CH8_FONT_ADDR+i] = font[i];
    }
    
    memory[0x1FF] = 5; // TEST SUITE VALUE
}

Chip8::~Chip8() {}

uint8_t Chip8::sdl_scancode_to_key(uint16_t scancode) {
    switch(scancode) {
        case SDL_SCANCODE_1: {
            return 0x1;
        }
        case SDL_SCANCODE_2: {
            return 0x2;
        }
        case SDL_SCANCODE_3: {
            return 0x3;
        }
        case SDL_SCANCODE_4: {
            return 0xC;
        }
        case SDL_SCANCODE_Q: {
            return 0x4;
        }
        case SDL_SCANCODE_W: {
            return 0x5;
        }
        case SDL_SCANCODE_E: {
            return 0x6;
        }
        case SDL_SCANCODE_R: {
            return 0xD;
        }
        case SDL_SCANCODE_A: {
            return 0x7;
        }
        case SDL_SCANCODE_S: {
            return 0x8;
        }
        case SDL_SCANCODE_D: {
            return 0x9;
        }
        case SDL_SCANCODE_F: {
            return 0xE;
        }
        case SDL_SCANCODE_Z: {
            return 0xA;
        }
        case SDL_SCANCODE_X: {
            return 0x0;
        }
        case SDL_SCANCODE_C: {
            return 0xB;
        }
        case SDL_SCANCODE_V: {
            return 0xF;
        }
    }
    return 0xFF;
}

inline void Chip8::increment_pc() {
    pc += CH8_PC_STEP;
}

void Chip8::load(std::vector<uint8_t>& program) {
    pc = CH8_PC_START;

    for(size_t i = 0; i < program.size(); i++) {
        memory[CH8_PC_START+i] = program[i];
    }
}

void Chip8::clear_fb() {
    for(size_t addr = 0; addr < framebuffer.size(); addr++) {
        framebuffer[addr] = CH8_SCREEN_COLOR_2;
    }
}

void* Chip8::get_fb() {
    return &framebuffer.__elems_;
}

void Chip8::set_key(uint8_t key, bool status) {
    keyboard[key] = status;
}

uint16_t Chip8::fetch_instruction() {
    return (memory[pc] << 8) | memory[pc+1];
}

bool Chip8::step() {
    auto ins = fetch_instruction();
    increment_pc();
    decode_instruction(ins);
    return pc >= CH8_MEMORY_SIZE;
}

void Chip8::run() {
    std::cout << "Chip8 started!" << std::endl;

    while(pc < CH8_MEMORY_SIZE) {
        step();
        std::this_thread::sleep_for(std::chrono::nanoseconds(1'000'000'000 / 500));
    }
}

void Chip8::pause() {

}

void Chip8::decode_instruction(uint16_t& instruction) {
    uint8_t instruction_type = instruction >> 12;

    uint8_t reg_x = (instruction >> 8) & 0xF;
    uint8_t reg_y = (instruction >> 4) & 0xF;

    uint16_t value12 = instruction & 0xFFF;
    uint8_t  value8  = instruction & 0xFF;
    uint8_t  value4  = instruction & 0xF;

    switch(instruction_type) {
        case 0x0: {
            switch(value8) {
                case 0xE0: {
                    clear_fb();
                    break;
                }
                case 0xEE: {
                    pc = stack.top();
                    stack.pop();
                    break;
                }
            }
            break;
        }
        case 0x2: {
            stack.push(pc);
        }
        case 0x1: {
            pc = value12;
            break;
        }
        case 0x3: {
            if(registers[reg_x] == value8) {
                increment_pc();
            }
            break;
        }
        case 0x4: {
            if(registers[reg_x] != value8) {
                increment_pc();
            }
            break;
        }
        case 0x5: {
            if(registers[reg_x] == registers[reg_y]) {
                increment_pc();
            }
            break;
        }
        case 0x6: {
            registers[reg_x] = value8;
            break;
        }
        case 0x7: {
            registers[reg_x] += value8;
            break;
        }
        case 0x8: {
            switch(value4) {
                case 0x0: {
                    registers[reg_x] = registers[reg_y];
                    break;
                }
                case 0x1: {
                    registers[reg_x] |= registers[reg_y];
                    break;
                }
                case 0x2: {
                    registers[reg_x] &= registers[reg_y];
                    break;
                }
                case 0x3: {
                    registers[reg_x] ^= registers[reg_y];
                    break;
                }
                case 0x4: {
                    uint16_t result = registers[reg_x] + registers[reg_y];
                    bool flag = result > 0xFF;
                    registers[reg_x] = result;
                    registers[CH8_FLAG_REG] = flag;
                    break;
                }
                case 0x5: {
                    bool flag = registers[reg_x] >= registers[reg_y];
                    registers[reg_x] -= registers[reg_y];
                    registers[CH8_FLAG_REG] = flag;
                    break;
                }
                case 0x7: {
                    bool flag =  registers[reg_y] >= registers[reg_x];
                    registers[reg_x] = registers[reg_y] - registers[reg_x];
                    registers[CH8_FLAG_REG] = flag;
                    break;
                }
                case 0x6: {
                    bool flag = registers[reg_y] & 1;
                    registers[reg_x] = registers[reg_y] >> 1;
                    registers[CH8_FLAG_REG] = flag;
                    break;
                }
                case 0xE: {
                    bool flag = registers[reg_x] >> 7;
                    registers[reg_x] = registers[reg_x] << 1;
                    registers[CH8_FLAG_REG] = flag;
                    break;
                }
            }
            break;
        }
        case 0x9: {
            if(registers[reg_x] != registers[reg_y]) {
                increment_pc();
            }
            break;
        }
        case 0xA: {
            index_register = value12;
            break;
        }
        case 0xB: {
            pc = value12 + registers[0];
            break;
        }
        case 0xC: {
            registers[reg_x] = rand() & value8;
            break;
        }
        case 0xD: {
            fb_modified = true;

            uint8_t x = registers[reg_x];
            uint8_t y = registers[reg_y];
            uint8_t height = value4;
            uint16_t sprite_addr = index_register;

            for(auto i = 0; i < height; i++) {
                std::bitset<8> sprite = memory[sprite_addr++];
                auto row = (y+i) % 32;
                for(auto j = 0; j < 8; j++) {
                    if(sprite[7-j]) {
                        auto column = (x+j) % 64;
                        auto position = column + row * CH8_SCREEN_WIDTH;
                        if(framebuffer[position] == CH8_SCREEN_COLOR) {
                            framebuffer[position] = CH8_SCREEN_COLOR_2;
                            registers[CH8_FLAG_REG] = true;
                        } else {
                            framebuffer[position] = CH8_SCREEN_COLOR;
                        }
                    }
                }
            }
            break;
        }
        case 0xE: {
            switch(value8) {
                case 0x9E: {
                    if(keyboard[registers[reg_x]]) {
                        increment_pc();
                    }
                    break;
                }
                case 0xA1: {
                    if(!keyboard[registers[reg_x]]) {
                        increment_pc();
                    }
                    break;
                }
            }
            break;
        }
        case 0xF: {
            switch(value8) {
                case 0x07: {
                    registers[reg_x] = delay_timer;
                    break;
                }
                case 0x0A: {
                    bool key_pressed = false;
                    for(size_t i = 0; i < keyboard.size(); i++) {
                        if(keyboard[i]) {
                            key_pressed = true;
                            registers[reg_x] = i;
                            break;
                        }
                    }
                    if(!key_pressed) {
                        pc -= CH8_PC_STEP;
                    }
                    break;
                }
                case 0x15: {
                    delay_timer = registers[reg_x];
                    break;
                }
                case 0x18: {
                    sound_timer = registers[reg_x];
                    break;
                }
                case 0x1E: {
                    index_register += registers[reg_x];
                    break;
                }
                case 0x29: {
                    auto location = registers[reg_x] & 0xF;
                    index_register = memory[CH8_FONT_ADDR + location];
                    break;
                }
                case 0x33: {
                    uint8_t num = registers[reg_x];
                    for(auto i = 2; i >= 0; i--) {
                        memory[index_register+i] = num % 10;
                        num = num / 10;
                    }
                    break;
                }
                case 0x55: {
                    for(auto i = 0; i <= reg_x; i++) {
                        memory[index_register+i] = registers[i];
                    }
                    break;
                }
                case 0x65: {
                    for(auto i = 0; i <= reg_x; i++) {
                        registers[i] = memory[index_register+i];
                    }
                    break;
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}