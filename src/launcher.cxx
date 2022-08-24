#include <iostream>
#include <thread>
#include <fstream>
#include <chrono>

#include "../include/launcher.hxx"
#include "../include/chip8.hxx"

Launcher::Launcher(std::string_view title, const int width, const int height) : window{}, renderer{}, texture{} {
    if(SDL_Init(SDL_INIT_VIDEO)) throw SDL_GetError();

    window = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if(!window) throw SDL_GetError();

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) throw SDL_GetError();

    SDL_RenderSetLogicalSize(renderer, CH8_SCREEN_WIDTH, CH8_SCREEN_HEIGHT);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_TARGET, CH8_SCREEN_WIDTH, CH8_SCREEN_HEIGHT);
    if(!texture) throw SDL_GetError();
}

Launcher::~Launcher() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

std::vector<uint8_t> Launcher::read_binary(std::string_view filename) const {
    std::ifstream ifstream{filename, std::ios::in | std::ios::binary};
    if(!ifstream.good()) {
        std::cout << "Binary could not be opened: " << filename << std::endl;
        return {};
    }
    return std::vector<uint8_t>{std::istreambuf_iterator<char>(ifstream), {}};
}

void Launcher::run(std::string_view filename) const {
    std::vector<uint8_t> program = read_binary(filename);

    Chip8 chip8;
    chip8.load(program);

    std::thread ch8_thread = chip8.run();

    SDL_Event event;
    bool is_running = true;

    while(is_running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    is_running = false;
                    break;
                case SDL_KEYDOWN:
                    chip8.set_key(event.key.keysym.scancode, true);
                    break;
                case SDL_KEYUP:
                    chip8.set_key(event.key.keysym.scancode, false);
                    break;
            }
        }

        if(void* fb = chip8.get_fb()) {
            SDL_UpdateTexture(texture, nullptr, fb, CH8_SCREEN_WIDTH);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
        }

        chip8.handle_timers();

        SDL_Delay(1000 / 60);
    }
}