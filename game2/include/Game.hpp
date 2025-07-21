
#pragma once
#include <SDL2/SDL.h>
#include<Board.hpp>

class Game {
public:
    bool init(const char* title, int width, int height);
    void run();
    void cleanup();
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    Board* board = nullptr;
    bool isRunning = true;
    void handleEvents();
    void update();
    void render();
};
