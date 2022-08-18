#include "../include/chip8.hxx"

#include <iostream>
#include <thread>
#include <chrono>

Chip8::Chip8() : index_register{}, registers{}, memory{}, stack{}, delay_timer{}, sound_timer{}, framebuffer{}, pc{} {
    std::cout << pc << std::endl;
    increment_pc();
    std::cout << pc << std::endl;

    for(auto i = 0; i < framebuffer.size(); i++) {
        if(i % 4 == 0) {
            framebuffer.flip(i);
        }
    }
}

Chip8::~Chip8() {
    std::cout << "Chip8 bye bye" << std::endl;
}

inline void Chip8::reset_pc() {
    pc = CH8_PC_START;
}

inline void Chip8::increment_pc() {
    pc += CH8_PC_STEP;
}

void Chip8::load(ByteCode& bytecode) {
    for(auto byte : bytecode) {
        std::cout << byte << " ";
    }
}

void Chip8::step() {
    std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
}

void Chip8::run() {
    std::cout << "Chip8 started!" << std::endl;

    while(true) {
        step();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Chip8::pause() {

}