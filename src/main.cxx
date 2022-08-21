#include <iostream>

#include "include/launcher.hxx"

int main() {
    Launcher launcher{"chip8-emulator", 640, 320};
    
    launcher.run("chip8-test-suite.ch8");

    return 0;
}