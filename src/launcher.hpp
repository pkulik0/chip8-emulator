//
// Created by pkulik0 on 18/08/2022.
//

#pragma once

#include <vector>
#include <string_view>

#include <SDL2/SDL.h>

class Launcher {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    static std::vector<uint8_t> read_binary(std::string_view filename) ;
public:
    Launcher(std::string_view title, const int width, const int height);
    ~Launcher();

    void run(std::string_view filename) const;
};