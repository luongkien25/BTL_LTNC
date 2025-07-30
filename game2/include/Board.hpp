#ifndef BOARD_HPP
#define BOARD_HPP
#include "Tile.hpp"
#include <iostream>
#include <SDL.h>
#include <string>
#include <vector>
#include "tinyxml2.h"
using namespace std;
using namespace tinyxml2;
enum BonusType {
    NONE,
    DOUBLE_LETTER,
    TRIPLE_LETTER,
    DOUBLE_WORD,
    TRIPLE_WORD
};

extern BonusType board_bonus[15][15]; 

class Board {
public:
    Board(SDL_Renderer* renderer);
    void  load(const string& tmxFile,SDL_Renderer* renderer);
    void render(int& screenW,int& screenH);
    void load_bonus_from_txt(const std::string& path);
    void update_board_with_tile(Tile* tile, int x, int y);
    SDL_Rect board_rect;
private:
    SDL_Renderer* renderer;
    SDL_Texture* tileset = nullptr;
    SDL_Texture* board_texture = nullptr;

    int tileWidth = 0;
    int tileHeight = 0;
    int mapWidth = 0;
    int mapHeight = 0;
    int board_size_x = 520;
    int board_size_y = 480;
    vector<int> tileIDs;
    
    Tile* board[15][15] = {{nullptr}}; // 2D array of Tile pointers
};


#endif

