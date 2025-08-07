#pragma once
#include "Graph.hpp"
#include"Tile.hpp"
#include"Board.hpp"
#include<iostream>
#include<SDL2/SDL.h>
#include<string>
#include<vector>
#include <utility>
#include<SDL2/SDL_ttf.h>

using namespace std;
#include <iomanip>
#include <sstream>
class Player{

 public:
     Player(SDL_Renderer* renderer);
     string formatTimeMMSS(Uint32 ms);
     void handle_timer(Uint32& current_time);
     void handleEvent(SDL_Event& event,int& mouseX,int& mouseY);
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
     bool is_first_player_turn = false;
     int score = 0,player1_score = 0,player2_score =0 ;
     string player1_score_text = "Score: ",player2_score_text = "Score: ";
 private:
     SDL_Renderer* renderer;
     SDL_Texture* rack;

     int letters_size = 7;
     int tile_size = 40;

     Uint32 player1_time_left;
     Uint32 player2_time_left;
     Uint32 time_per_match = 20000;
};
