#include "Tile.hpp"
#include <vector>
#include <random>
#include <algorithm>

class TileBag {
private:
    std::vector<Tile> tiles;
    std::mt19937 rng;
    

    struct TileInfo {
        char letter;
        int count;
        int score;
    };
    
    static const TileInfo TILE_DISTRIBUTION[];
    static const int TOTAL_TILES = 100;
    
public:
    TileBag();
    ~TileBag();
    
  
    void initializeBag();
    

    Tile drawTile();
    

    std::vector<Tile> drawTiles(int count);
    

    void returnTiles(const std::vector<Tile>& returnedTiles);
    

    void shuffle();
    

    bool isEmpty() const;
    

    int getRemainingCount() const;
    

    int getLetterCount(char letter) const;
    

    void reset();
};
