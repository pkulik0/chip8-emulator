#include <iostream>
#include <thread>
#include <fstream>

#include "../include/emulator.hxx"
#include "../include/chip8.hxx"

Emulator::Emulator(std::string title, int width, int height) : window{}, renderer{}, texture{}, is_running{false} {
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if(!window) {
        std::cout << "Could not create a window: " << SDL_GetError() << std::endl;
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        std::cout << "Could not create a renderer: " << SDL_GetError() << std::endl;
        return;
    }
    SDL_RenderSetLogicalSize(renderer, CH8_SCREEN_WIDTH, CH8_SCREEN_HEIGHT);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_TARGET, CH8_SCREEN_WIDTH, CH8_SCREEN_HEIGHT);
    if(!texture) {
        std::cout << "Could not create a texture: " << SDL_GetError() << std::endl;
        return;
    }
}

Emulator::~Emulator() {
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

std::vector<uint8_t> Emulator::read_binary(std::string& filename) {
    std::ifstream ifstream{filename, std::ios::in | std::ios::binary};
    if(!ifstream.is_open()) {
        std::cout << "err" << std::endl;
        return {};
    }
    return std::vector<uint8_t>{std::istreambuf_iterator<char>(ifstream), {}};
}

void Emulator::run(std::string filename) {
    std::vector<uint8_t> program = read_binary(filename);

    Chip8 chip8;
    chip8.load(program);
    // std::thread chip8_thread{&Chip8::run, chip8};
    chip8.run();

    SDL_Event event;
    is_running = true;

    size_t steps = 0;
    while(is_running) {
        if(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    is_running = false;
                    break;
                case SDL_KEYDOWN:
                    std::cout << "Step " << steps++ << std::endl;
                    chip8.step();
                    break;
                default:
                    break;
            }
        }

        auto c = CH8_SCREEN_COLOR_2
        SDL_SetRenderDrawColor(renderer, c, c, c, 0xFF);
        SDL_RenderClear(renderer);

        SDL_UpdateTexture(texture, nullptr, chip8.framebuffer.__elems_, CH8_SCREEN_WIDTH);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }
}