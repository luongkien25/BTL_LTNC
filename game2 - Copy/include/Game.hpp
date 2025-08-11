
#pragma once
#include <SDL2/SDL.h>
#include<Board.hpp>
#include "Player.hpp"
#include "menu.hpp"
#include "UI.hpp"

class Game {
public:
    bool init(const char* title, int width, int height);
    void run();
    void cleanup();
    int screenW, screenH;
    Uint32 current_time = SDL_GetTicks();
    bool is_showing_menu = false;
    bool is_showing_win_effect = false;
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    Menu* menu = nullptr;
    Board* board = nullptr;
    Player* player = nullptr;
    UI* ui = nullptr;
    int game_state = 0;
    bool isRunning = true;
    bool is_clicked_submit = false;

    void handleEvents();

    void update();
    void render();
};
