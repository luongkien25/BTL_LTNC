#ifndef TILEBAG_HPP
#define TILEBAG_HPP

#include <vector>
#include <random>
#include <algorithm>
#include "Tile.hpp"

struct TileBagTile {
    char letter;
    int score;
    int count;
};

class TileBag {
private:
    std::vector<TileBagTile> availableTiles;
    std::vector<char> bag;
    std::mt19937 rng;

    void initializeStandardTiles();
    void fillBag();

public:
    TileBag();

    // Lấy tile ngẫu nhiên từ bag
    char drawTile();

    // Lấy nhiều tile cùng lúc
    std::vector<char> drawTiles(int count);

    // Trả tile về bag (dùng cho exchange)
    void returnTiles(const std::vector<char>& tiles);

    // Kiểm tra còn bao nhiêu tile trong bag
    int getRemainingCount() const;

    // Kiểm tra bag có còn tile không
    bool isEmpty() const;

    // Lấy điểm của một chữ cái
    int getTileScore(char letter) const;

    // Shuffle bag
    void shuffle();

    // Reset bag về trạng thái ban đầu
    void reset();

    // Debug: hiển thị nội dung bag
    void printBagContents() const;
};

#endif // TILEBAG_HPP
