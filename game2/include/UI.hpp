#pragma once
#include<iostream>
#include<SDL2/SDL.h>
#include<SDL_image.h>
using namespace std;

class UI{
  public:
     void render_buttons(string path,SDL_Rect dstRect);
     UI(SDL_Renderer* renderer);
     SDL_Rect submitRect = {540,750,76,43};
  private:
    SDL_Renderer* renderer;
};
