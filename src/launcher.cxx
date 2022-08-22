#include <iostream>
#include <thread>
#include <fstream>
#include <chrono>

#include "../include/launcher.hxx"
#include "../include/chip8.hxx"

Launcher::Launcher(const std::string_view& title, const int width, const int height) : window{}, renderer{}, texture{}, is_running{false} {
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

std::vector<uint8_t> Launcher::read_binary(const std::string_view& filename) {
    std::ifstream ifstream{filename, std::ios::in | std::ios::binary};
    if(!ifstream.good()) {
        std::cout << "Binary could not be opened: " << filename << std::endl;
        return {};
    }
    return std::vector<uint8_t>{std::istreambuf_iterator<char>(ifstream), {}};
}

void Launcher::run(const std::string_view& filename) {
    std::vector<uint8_t> program = read_binary(filename);

    Chip8* chip8 = new Chip8;
    chip8->load(program);

    std::thread ch8_thread = chip8->run();
    ch8_thread.detach();

    SDL_Event event;
    is_running = true;

    while(is_running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: {
                    is_running = false;
                    break;
                }
                case SDL_KEYDOWN: {
                    chip8->set_key(event.key.keysym.scancode, true);
                    break;
                }
                case SDL_KEYUP: {
                    chip8->set_key(event.key.keysym.scancode, false);
                    break;
                }
            }
        }

        if(void* fb = chip8->get_fb()) {
            auto c = CH8_SCREEN_COLOR_OFF;
            SDL_SetRenderDrawColor(renderer, c, c, c, 0xFF);
            SDL_RenderClear(renderer);

            SDL_UpdateTexture(texture, nullptr, fb, CH8_SCREEN_WIDTH);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);

            SDL_RenderPresent(renderer);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}