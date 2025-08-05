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
BonusType board_bonus[15][15];
Board::Board(SDL_Renderer* renderer) : renderer(renderer) {
    // Optional: initialization logic here
}

void Board::load_bonus_from_txt(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        SDL_Log("Failed to load board layout: %s", path.c_str());
        return;
    }

    std::string line;
    for (int row = 0; row < 15 && std::getline(file, line); row++) {
        for (int col = 0; col < 15 && col < (int)line.size(); col++) {
            switch (line[col]) {
                case 't': board_bonus[row][col] = TRIPLE_WORD; break;
                case 'd': board_bonus[row][col] = DOUBLE_WORD; break;
                case '2': board_bonus[row][col] = DOUBLE_LETTER; break;
                case '3': board_bonus[row][col] = TRIPLE_LETTER; break;
                default:  board_bonus[row][col] = NONE; break;
            }
        }
    }
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
            board_rect = {screenW/2-33,screenH/2-board_size_y,board_size_x,board_size_y};
            SDL_RenderCopy(renderer, board_texture,nullptr, &board_rect);
}

void Board::update_board_with_tile(Tile* tile, int x, int y) {

    if (boardTile[x][y] == nullptr) {
        boardTile[x][y] = tile;
        SDL_Log("Tile  at position (%d, %d)", x, y);
    } else {
        SDL_Log("Tile already exists at position (%d, %d)", x, y);
    }
}
