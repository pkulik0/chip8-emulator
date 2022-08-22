#include <iostream>

#include "../include/launcher.hxx"

int main(int argc, char** argv) {
    srand(time(0));

    if(argc < 2) {
        std::cout << "Usage:\n";
        std::cout << argv[0] << " <path_to_image>\n" << std::endl;
        return 1;
    }
    std::string filename{argv[1]};

    Launcher launcher{"chip8-emulator", 640, 320};
    launcher.run(filename);

    return 0;
}