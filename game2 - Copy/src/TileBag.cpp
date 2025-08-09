
#include "TileBag.hpp"
#include <iostream>
#include <chrono>

TileBag::TileBag() {
    // Initialize random number generator với seed từ thời gian hiện tại
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    rng.seed(seed);

    initializeStandardTiles();
    fillBag();
    shuffle();
}

void TileBag::initializeStandardTiles() {
    // Cấu hình tile theo chuẩn Scrabble tiếng Anh
    availableTiles = {
        {'A', 1, 9},   {'B', 3, 2},   {'C', 3, 2},   {'D', 2, 4},
        {'E', 1, 12},  {'F', 4, 2},   {'G', 2, 3},   {'H', 4, 2},
        {'I', 1, 9},   {'J', 8, 1},   {'K', 5, 1},   {'L', 1, 4},
        {'M', 3, 2},   {'N', 1, 6},   {'O', 1, 8},   {'P', 3, 2},
        {'Q', 10, 1},  {'R', 1, 6},   {'S', 1, 4},   {'T', 1, 6},
        {'U', 1, 4},   {'V', 4, 2},   {'W', 4, 2},   {'X', 8, 1},
        {'Y', 4, 2},   {'Z', 10, 1},  {'*', 0, 2}    // * là blank tile
    };
}

void TileBag::fillBag() {
    bag.clear();
    for (const auto& tileInfo : availableTiles) {
        for (int i = 0; i < tileInfo.count; i++) {
            bag.push_back(tileInfo.letter);

        }
    }
}

void TileBag::shuffle() {
    std::shuffle(bag.begin(), bag.end(), rng);
}

char TileBag::drawTile() {
    if (bag.empty()) {
        return '\0'; // Trả về null character nếu bag rỗng
    }

    char tile = bag.back();
    bag.pop_back();
    return tile;
}

std::vector<char> TileBag::drawTiles(int count) {
    std::vector<char> drawnTiles;

    for (int i = 0; i < count && !bag.empty(); i++) {
        drawnTiles.push_back(drawTile());
    }

    return drawnTiles;
}

void TileBag::returnTiles(const std::vector<char>& tiles) {
    for (char tile : tiles) {
        bag.push_back(tile);
    }
    shuffle(); // Shuffle lại sau khi trả tile
}

int TileBag::getRemainingCount() const {
    return bag.size();
}

bool TileBag::isEmpty() const {
    return bag.empty();
}

int TileBag::getTileScore(char letter) const {
    for (const auto& tileInfo : availableTiles) {
        if (tileInfo.letter == letter) {
            return tileInfo.score;
        }
    }
    return 0; // Trả về 0 nếu không tìm thấy
}

void TileBag::reset() {
    fillBag();
    shuffle();
}

void TileBag::printBagContents() const {
    std::cout << "Bag contents (" << bag.size() << " tiles): ";
    for (size_t i = 0; i < bag.size(); i++) {
        std::cout << bag[i];
        if (i < bag.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;
}
