#ifndef TILE_HPP
#define TILE_HPP

#include <SDL2/SDL.h>
#include <string>
struct Tile{
    int tile_size = 40;
    SDL_Rect rect;
    int score;
    char letter;
    bool selected = false;
};

#endif // TILE_HPP
