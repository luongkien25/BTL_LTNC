#pragma once
#include<iostream>
#include<SDL2/SDL.h>
#include<string>
#include<vector>
#include<SDL2/SDL_ttf.h>
using namespace std;
struct Tile{
    int tile_size = 40;
    SDL_Rect rect;
    int score;
    char letter;
    bool selected = false;
};

class Player{

 public:
     Player(SDL_Renderer* renderer);
     void load_player_rack(const string& path);
     void generate_letters();
     void rack_update();
     void render();
     void renderText(int x,int y,TTF_Font* font,string text);
     vector<Tile> letters;
     vector<Tile> player2_letters;
     SDL_Rect rack_rect;
     SDL_Rect dst_rect;
 private:
     SDL_Renderer* renderer;
     SDL_Texture* rack;

     int letters_size = 7;
     int tile_size = 40;

};
