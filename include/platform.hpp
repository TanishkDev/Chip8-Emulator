#pragma once
#ifndef PLATFORM_H
#define PLATFROM_H

#include <SDL2/SDL.h>
#include <SDL_render.h>
#include <cstdint>

class platform{
public:
    platform(char const* title, int wWidth, int wHeight, int tWidth, int tHeight);
    ~platform();
    void update(void const* buffer, int pitch);
    bool processInput(uint8_t* keys);
private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};
};
#endif
