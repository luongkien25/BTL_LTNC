#ifndef HINTSYSTEM_HPP
#define HINTSYSTEM_HPP

#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include "Tile.hpp"
#include "Board.hpp"
#include "Graph.hpp"

using namespace std;

struct HintMove {
    string word;
    int score;
    int startX, startY;
    bool isHorizontal;
    vector<pair<int, int>> positions;
    vector<char> tilesUsed; // Tiles từ rack cần sử dụng
    
    HintMove() : score(0), startX(-1), startY(-1), isHorizontal(true) {}
};

class HintSystem {
public:
    HintSystem();
    
    // Main hint function - tìm các nước đi tốt nhất
    vector<HintMove> generateHints(Tile* Board[15][15], const vector<Tile>& playerRack, int maxHints = 5);
    
    // Tìm tất cả vị trí anchor (có thể đặt từ mới)
    vector<pair<int, int>> findAnchorPositions(Tile* Board[15][15]);
    
    // Tạo tất cả hoán vị có thể từ rack
    vector<string> generatePermutations(const vector<char>& letters, int maxLength = 7);
    
    // Kiểm tra xem có thể đặt từ tại vị trí không
    bool canPlaceWordAt(Tile* Board[15][15], const string& word, int x, int y, bool horizontal, 
                       const vector<char>& availableLetters, HintMove& move);
    
    // Tính điểm cho một từ tại vị trí cụ thể
    int calculateWordScore(Tile* Board[15][15], const string& word, int x, int y, bool horizontal,
                          const vector<pair<int, int>>& newTilePositions);
    
    // Kiểm tra xem tất cả từ tạo ra có hợp lệ không
    bool validateAllFormedWords(Tile* Board[15][15], const vector<pair<int, int>>& newTilePositions,
                               const string& mainWord);

    // ======== THÊM CÁC FUNCTION DECLARATIONS THIẾU ========
    void generateFirstMoveHints(Tile* Board[15][15], const vector<char>& availableLetters, vector<HintMove>& hints);
    
    void generateHintsAtPosition(Tile* Board[15][15], const vector<char>& availableLetters, 
                               int anchorX, int anchorY, vector<HintMove>& hints);

private:
    Graph* dictionary;
    
    // Helper methods
    bool isValidPlacement(Tile* Board[15][15], int x, int y, char letter, bool horizontal,
                         bool isFirstTile, bool isLastTile);
    vector<string> findCrossWords(Tile* Board[15][15], int x, int y, char newLetter);
    bool hasAdjacentTile(Tile* Board[15][15], int x, int y);
    int getTileScore(char letter);
};

#endif // HINTSYSTEM_HPP