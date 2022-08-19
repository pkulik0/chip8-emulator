#pragma once

#include <SDL2/SDL.h>

class Emulator {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    bool is_running;

    std::vector<uint8_t> read_binary(std::string& filename);
public:
    Emulator(std::string title, int width, int height);
    ~Emulator();

    void run(std::string filename);
};