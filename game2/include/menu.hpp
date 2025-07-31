#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <vector>
#include <SDL2/SDL_ttf.h>
using namespace std;

struct Option{
   string option;
   SDL_Rect dstRect;
   bool selected = false;

};

class Menu{
   public:
       Menu(SDL_Renderer* renderer);
       void render();
       void renderText(int x,int y,TTF_Font* font,string text);
       void handleEvent(SDL_Event& event,int& mouseX,int& mouseY);
       vector<Option> Option;
       bool test = false;
   private:
      SDL_Renderer* renderer;
};

