#pragma once
#include<iostream>
#include <SDL.h>
#include<string>
#include<vector>
#include "tinyxml2.h"
using namespace std;
class Board {
public:
    Board(SDL_Renderer* renderer);
    void  load(const string& tmxFile,SDL_Renderer* renderer);
    void render(int& screenW,int& screenH);
    SDL_Rect board_rect;
private:
    SDL_Renderer* renderer;
    SDL_Texture* tileset = nullptr;
    SDL_Texture* board_texture = nullptr;

    int tileWidth = 0;
    int tileHeight = 0;
    int mapWidth = 0;
    int mapHeight = 0;
    int board_size_x = 530;
    int board_size_y = 480;
    vector<int> tileIDs;
};


