#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <Board.hpp>
#include "tinyxml2.h"
#include <SDL_image.h>
#include <string>
#include <filesystem>
#include <sstream>
#include <Game.hpp>
using namespace std;
using namespace tinyxml2;

Board::Board(SDL_Renderer* renderer) : renderer(renderer) {
    // Optional: initialization logic here
}

void Board::load(const string& file, SDL_Renderer* renderer) {
    SDL_Surface* tempSurface = IMG_Load(file.c_str());
    if (!tempSurface) {
        SDL_Log("Failed to load image %s: %s", file.c_str(), IMG_GetError());
        return;
    }

    board_texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    if (!board_texture) {
        SDL_Log("Failed to create texture from %s: %s", file.c_str(), SDL_GetError());
    }


}

void Board::render(int& screenW,int& screenH) {
            board_rect = {screenW/2,screenH/2-board_size_y,board_size_x,board_size_y};
            SDL_RenderCopy(renderer, board_texture,nullptr, &board_rect);
}


