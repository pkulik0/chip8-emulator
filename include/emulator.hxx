#pragma once

#include <string>
#include <SDL2/SDL.h>

class Emulator {
private:
    SDL_Window* window;
    SDL_Surface* surface;
    SDL_Event event;

    bool is_running;
public:
    Emulator(std::string title, int width, int height);
    ~Emulator();

    void run();
};