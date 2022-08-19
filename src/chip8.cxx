#include <iostream>
#include <cstdio>

#include <thread>
#include <chrono>

#include "../include/chip8.hxx"

Chip8::Chip8() : index_register{}, registers{}, memory{}, stack{}, delay_timer{}, sound_timer{}, pc{}, framebuffer{} {
    memory[0x1FF] = 2;
}

Chip8::~Chip8() {}

inline void Chip8::reset_pc() {
    pc = CH8_PC_START;
}

inline void Chip8::increment_pc() {
    pc += CH8_PC_STEP;
}

void Chip8::load(std::vector<uint8_t>& program) {
    reset_pc();

    for(auto i = 0; i < program.size(); i++) {
        memory[CH8_PC_START+i] = program[i];
    }
}

void Chip8::clear_fb() {
    for(uint16_t addr = 0; addr < framebuffer.size(); addr++) {
        framebuffer[addr] = CH8_SCREEN_COLOR_2;
    }
}

uint16_t Chip8::fetch_instruction() {
    return (memory[pc] << 8) | memory[pc+1];
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
        case 0x1: {
            pc = value12;
            break;
        }
        case 0x2: {
            stack.push(pc);
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
        case 0xD: {
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
                            registers[0xF] = true;
                        } else {
                            framebuffer[position] = CH8_SCREEN_COLOR;
                        }
                    }
                }
            }
            break;
        }
        case 0xF: {
            uint16_t addr = index_register;
            for(auto i = 0; i <= reg_x; i++) {
                registers[i] = memory[addr++];
            }
            break;
        }
        default: {
            printf("\tnot implemented: %x, full: %x\n", instruction_type, instruction);
            break;
        }
    }
}

void Chip8::step() {
    auto ins = fetch_instruction();
    increment_pc();
    decode_instruction(ins);
}

void Chip8::run() {
    std::cout << "Chip8 started!" << std::endl;

    const int steps = 39;
    for(auto i = 0; i < steps; i++) {
        step();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Chip8::pause() {

}