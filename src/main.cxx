#include <iostream>

#include "../include/emulator.hxx"

int main() {
    std::cout << "Chip8 Emulator Init" << std::endl;
    Emulator emulator{"chip8-emulator", 640, 320};
    
    emulator.run("chip8-test-suite.ch8");

    return 0;
}