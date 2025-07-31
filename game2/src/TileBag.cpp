#include "TileBag.hpp"
#include <chrono>
#include <iostream>


const TileBag::TileInfo TileBag::TILE_DISTRIBUTION[] = {
    {'A', 9, 1}, {'B', 2, 3}, {'C', 2, 3}, {'D', 4, 2}, {'E', 12, 1},
    {'F', 2, 4}, {'G', 3, 2}, {'H', 2, 4}, {'I', 9, 1}, {'J', 1, 8},
    {'K', 1, 5}, {'L', 4, 1}, {'M', 2, 3}, {'N', 6, 1}, {'O', 8, 1},
    {'P', 2, 3}, {'Q', 1, 10}, {'R', 6, 1}, {'S', 4, 1}, {'T', 6, 1},
    {'U', 4, 1}, {'V', 2, 4}, {'W', 2, 4}, {'X', 1, 8}, {'Y', 2, 4},
    {'Z', 1, 10}, {'*', 2, 0} 
};

TileBag::TileBag() {
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    rng.seed(static_cast<unsigned int>(seed));
    
    initializeBag();
}

TileBag::~TileBag() {

}

void TileBag::initializeBag() {
    tiles.clear();
    tiles.reserve(TOTAL_TILES);
    

    for (const auto& tileInfo : TILE_DISTRIBUTION) {
        for (int i = 0; i < tileInfo.count; i++) {
            Tile tile;
            tile.letter = tileInfo.letter;
            tile.score = tileInfo.score;
            tile.selected = false;
            tile.rect = {0, 0, 40, 40}; 
            tiles.push_back(tile);
        }
    }
    
    shuffle();
}

Tile TileBag::drawTile() {
    if (isEmpty()) {
        Tile emptyTile;
        emptyTile.letter = '\0';
        emptyTile.score = 0;
        emptyTile.selected = false;
        emptyTile.rect = {0, 0, 40, 40};
        return emptyTile;
    }
    
    std::uniform_int_distribution<int> dist(0, tiles.size() - 1);
    int index = dist(rng);
    
    Tile drawnTile = tiles[index];
    

    tiles.erase(tiles.begin() + index);
    
    return drawnTile;
}

std::vector<Tile> TileBag::drawTiles(int count) {
    std::vector<Tile> drawnTiles;
    drawnTiles.reserve(count);
    
    for (int i = 0; i < count && !isEmpty(); i++) {
        drawnTiles.push_back(drawTile());
    }
    
    return drawnTiles;
}

void TileBag::returnTiles(const std::vector<Tile>& returnedTiles) {
    for (const auto& tile : returnedTiles) {
        Tile resetTile = tile;
        resetTile.selected = false;
        resetTile.rect = {0, 0, 40, 40};
        
        tiles.push_back(resetTile);
    }
    

    shuffle();
}

void TileBag::shuffle() {
    std::shuffle(tiles.begin(), tiles.end(), rng);
}

bool TileBag::isEmpty() const {
    return tiles.empty();
}

int TileBag::getRemainingCount() const {
    return static_cast<int>(tiles.size());
}

int TileBag::getLetterCount(char letter) const {
    int count = 0;
    for (const auto& tile : tiles) {
        if (tile.letter == letter) {
            count++;
        }
    }
    return count;
}

void TileBag::reset() {
    initializeBag();
}