#pragma once
#include<iostream>
#include <SDL.h>
#include<string>
#include<vector>
using namespace std;
class Board {
public:
    Board(SDL_Renderer* renderer);
    bool load(const string& tmxFile, const string& tilesetImage);
    void render();
private:
    SDL_Renderer* renderer;
    SDL_Texture* tileset = nullptr;
    int tileWidth = 0;
    int tileHeight = 0;
    int mapWidth = 0;
    int mapHeight = 0;
    vector<int> tileIDs;
};


