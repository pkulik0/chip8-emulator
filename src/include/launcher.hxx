#pragma once

#include <vector>
#include <string>

#include <SDL2/SDL.h>

class Launcher {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    bool is_running;

    std::vector<uint8_t> read_binary(const std::string& filename);
public:
    Launcher(const std::string& title, const int& width, const int& height);
    ~Launcher();

    void run(const std::string& filename);
};