#pragma once
#include<iostream>
#include<SDL2/SDL.h>
#include<SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
using namespace std;

struct Particle {
    float x, y;
    float vx, vy;
    SDL_Color color;
    int lifetime; // milliseconds
};

// Functions to display UI effects
void showWinningEffect(SDL_Renderer* renderer, TTF_Font* font, int screenW, int screenH,string&winner);

// Internal helper functions (not usually called directly from outside)
void spawnConfetti(std::vector<Particle>& particles, int screenW);
void spawnSparkles(std::vector<Particle>& particles, int centerX, int centerY);
void updateParticles(std::vector<Particle>& particles, int delta);
void renderParticles(SDL_Renderer* renderer, const std::vector<Particle>& particles);

class UI{
  public:
     void render_buttons(string path,SDL_Rect dstRect);
     UI(SDL_Renderer* renderer);
     SDL_Rect submitRect = {540,750,76,43};
  private:
    SDL_Renderer* renderer;
};
