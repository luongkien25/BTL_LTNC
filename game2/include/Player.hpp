#pragma once
#include "Graph.hpp"
#include"Tile.hpp"
#include"Board.hpp"
#include<iostream>
#include<SDL2/SDL.h>
#include<string>
#include<vector>
#include <utility>
#include "Constants.hpp"
#include<SDL2/SDL_ttf.h>

using namespace std;

class Player{

 public:
     Player(SDL_Renderer* renderer);
     void load_player_rack(const string& path);
     void generate_letters();
     void rack_update();
     void render();
     void renderText(int x,int y,TTF_Font* font,string text);
     void set_score(int score) { this->score = score; }
     int get_score() const { return score; }
     bool isVertical(const vector<pair<int, int>>& tile_positions);
     int calculateTileScore(int x, int y, Tile* tile);
     int getTileScore(char c);
     std::pair<bool, int> canSubmitAndCalculateScore(Tile* Board[15][15], vector<pair<int, int>> tile_positions);
     vector<Tile> letters;
     vector<Tile> player2_letters;///?
     std::vector<std::pair<int, int>> tile_positions;
     SDL_Rect rack_rect;
     SDL_Rect dst_rect;
     Graph * graph = Graph::getInstance();
 private:
     SDL_Renderer* renderer;
     SDL_Texture* rack;

     int letters_size = 7;
     int tile_size = 40;
     int score = 0;
};
