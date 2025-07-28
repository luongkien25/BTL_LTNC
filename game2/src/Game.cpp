#include "Game.hpp"
#include "Board.hpp"
#include "Player.hpp"
#include <iostream>
#include <tinyxml2.h>
#include<SDL2/SDL_ttf.h>
using namespace tinyxml2;
bool Game::init(const char* title, int width, int height) {
    TTF_Init();
    board = new Board(renderer);


    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!window) return false;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_GetRendererOutputSize(renderer, &screenW, &screenH);
    if (!renderer) return false;


    board = new Board(renderer);
    board->load("board.png",renderer);
    player = new Player(renderer);
    player->load_player_rack("rack.png");
    return true;
}

void Game::run() {
    player->generate_letters();
    while (isRunning) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}

void Game::cleanup() {

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) isRunning = false;
        else if(e.type == SDL_MOUSEBUTTONDOWN){
             int x = e.button.x;
             int y = e.button.y;
             SDL_Point mousePoint = {x, y};
             bool check_tile = false;
             for(auto& tile: player->letters){
                 if(SDL_PointInRect(&mousePoint, &tile.rect)){
                     tile.selected = true;
                     check_tile = true;
                     break;
                 }
             }
              if (!check_tile && SDL_PointInRect(&mousePoint, &board->board_rect)) {
                 mousePoint.x = ((mousePoint.x-board->board_rect.x)/48)*48+(board->board_rect.x);
                 mousePoint.y = ((mousePoint.y-board->board_rect.y)/48)*48+(board->board_rect.y);
                for (auto& tile : player->letters) {
                    if (tile.selected) {
                        tile.rect.x = mousePoint.x;
                        tile.rect.y = mousePoint.y;
                        tile.selected = false;
                        break;
                    }
                }
            }
        }
    }
}

void Game::update() {

}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    board->render(screenH,screenW);
    player->render();
    SDL_RenderPresent(renderer);
}
