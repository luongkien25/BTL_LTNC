
#pragma once
#include <SDL2/SDL.h>
#include<Board.hpp>
#include "Player.hpp"
#include "menu.hpp"


class Game {
public:
    bool init(const char* title, int width, int height);
    void run();
    void cleanup();
    int screenW, screenH;
    std::vector<std::pair<int, int>> tile_positions;
    BonusType board_bonus[15][15];

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    Menu* menu = nullptr;
    Board* board = nullptr;
    Player* player = nullptr;
    int game_state = 0;
    bool isRunning = true;

    void handleEvents();

    void update();
    void render();
};
