#include <iostream>
#include <thread>
#include <SDL2/SDL.h>

#include "../include/emulator.hxx"

Emulator::Emulator(std::string title, int width, int height) : window{}, surface{}, event{}, is_running{false} {
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "Could not initialize SDL." << std::endl;
        return;
    }

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

    if(!window) {
        std::cout << "Could not create a window." << std::endl;
        return;
    }
    surface = SDL_GetWindowSurface(window);
}

Emulator::~Emulator() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Emulator::run() {
    is_running = true;
    uint8_t i = 0;

    while(is_running) {
        if(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    is_running = false;
                    break;
                default:
                    break;
            }
        }

        SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0, 0, i++));
        SDL_UpdateWindowSurface(window);

        SDL_Delay(16);
    }
}