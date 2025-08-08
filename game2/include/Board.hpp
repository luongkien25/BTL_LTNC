#pragma once
#include<iostream>
#include "Tile.hpp"
#include <SDL.h>
#include<string>
#include<vector>
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
    ~Board();
    void load(const string& tmxFile, SDL_Renderer* renderer);
    void render(int& screenW, int& screenH, SDL_Texture* rackTexture = nullptr);
    void renderWithSelection(int& screenW, int& screenH, SDL_Texture* rackTexture, 
                           bool hasSelection, int selectedX, int selectedY);
    void load_bonus_from_txt(const std::string& path);
    void update_board_with_tile(Tile* tile, int x, int y);
    
    Tile* boardTile[15][15] = {{nullptr}};
    SDL_Rect board_rect;
    
private:
    SDL_Renderer* renderer;
    SDL_Texture* tileset = nullptr;
    SDL_Texture* board_texture = nullptr;

    int tileWidth = 0;
    int tileHeight = 0;
    int mapWidth = 0;
    int mapHeight = 0;
    int board_size_x = 600;
    int board_size_y = 600;
    vector<int> tileIDs;
};