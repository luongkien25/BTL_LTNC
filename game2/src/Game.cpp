#include "Game.hpp"
#include "Board.hpp"
#include "Player.hpp"

#include <iostream>
#include <tinyxml2.h>
#include<SDL2/SDL_ttf.h>
using namespace std;
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
    menu = new Menu(renderer);
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
        int mouseX,mouseY;
        SDL_GetMouseState(&mouseX,&mouseY);

        if (e.type == SDL_QUIT) isRunning = false;
        else if(e.type == SDL_MOUSEBUTTONDOWN){
             menu->handleEvent(e,mouseX,mouseY);
             int x = mouseX;
             int y = mouseY;
             SDL_Point mousePoint = {x, y};
             bool check_tile = false;
             for(auto& tile: player->letters){
                 if(SDL_PointInRect(&mousePoint, &tile.rect)){
                     tile.selected = true;
                     check_tile = true;
                     break;
                 }
             }

             bool player2_check_tile = false;
             for(auto& tile: player->player2_letters){
                 if(SDL_PointInRect(&mousePoint, &tile.rect)){
                     tile.selected = true;
                     player2_check_tile = true;
                     break;
                 }
             }

              if (!check_tile && SDL_PointInRect(&mousePoint, &board->board_rect)) {
                 mousePoint.x = ((mousePoint.x-board->board_rect.x)/40)*40+(board->board_rect.x);
                 mousePoint.y = ((mousePoint.y-board->board_rect.y)/40)*40+(board->board_rect.y);
                for (auto& tile : player->letters) {
                    if (tile.selected) {
                        tile.rect.x = mousePoint.x;
                        tile.rect.y = mousePoint.y;
                        int x_index = (mousePoint.x - board->board_rect.x) / 40;
                        int y_index = (mousePoint.y - board->board_rect.y) / 40;

                        board->update_board_with_tile(&tile, x_index, y_index);
                        player->tile_positions.push_back({x_index, y_index});
                        tile.selected = false;
                        break;
                    }
                }
            }

             if (!player2_check_tile && SDL_PointInRect(&mousePoint, &board->board_rect)) {
                 mousePoint.x = ((mousePoint.x-board->board_rect.x)/40)*40+(board->board_rect.x);
                 mousePoint.y = ((mousePoint.y-board->board_rect.y)/40)*40+(board->board_rect.y);
                for (auto& tile : player->player2_letters) {
                    if (tile.selected) {
                        tile.rect.x = mousePoint.x;
                        tile.rect.y = mousePoint.y;
                        int x_index = (mousePoint.x - board->board_rect.x) / 40;
                        int y_index = (mousePoint.y - board->board_rect.y) / 40;
                        
                        board->update_board_with_tile(&tile, x_index, y_index);
                        //player2 -> tile.position.....
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

    for(auto&opt:menu->Option){
        if(opt.selected){
            if(opt.option=="Multiple Players"){
                game_state = 1;

            }
        }
    }
    ;
    switch(game_state){
       case 0: menu->render();
       break;
       case 1:
           board->render(screenH,screenW);
           player->render();
           break;
    }



    SDL_RenderPresent(renderer);
}
