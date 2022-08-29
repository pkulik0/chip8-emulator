#include <iostream>

#include "../include/launcher.hxx"

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "Usage:\n" <<  argv[0] << " <path_to_image>\n" << std::endl;
        return EXIT_FAILURE;
    }
    std::string filename{argv[1]};

    try {
        Launcher launcher{"chip8-emulator", 640, 320};
        launcher.run(filename);
    } catch(std::runtime_error& error) {
        std::cout << "Something went wrong!\n" << error.what() << std::endl;
    }

    return EXIT_SUCCESS;
}