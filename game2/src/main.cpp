// main.cpp
#include <SDL2/SDL.h>
#include "Game.hpp"

int main(int argc, char* argv[]) {
    Game game;
    if (!game.init("Scrabble", 800, 800)) return -1;
    game.run();
    game.cleanup();
    return 0;
}
