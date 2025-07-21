#include "Game.hpp"
#include "Board.hpp"
#include <iostream>
#include <tinyxml2.h>
using namespace tinyxml2;
bool Game::init(const char* title, int width, int height) {
    board = new Board(renderer);
    board->load("Assets/board.tmx", "Assets/tiles.png");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window) return false;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;
    board = new Board(renderer);
    return true;
}

void Game::run() {
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
    }
}

void Game::update() {
    // Game logic here
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    board->render();
    SDL_RenderPresent(renderer);
}
