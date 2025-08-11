#pragma once
#include "Graph.hpp"
#include "Tile.hpp"
#include "Board.hpp"
#include "TileBag.hpp"
#include "HintSystem.hpp"  // ======== THÊM INCLUDE =======
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <iomanip>
#include <sstream>
#include <set>
#include <algorithm>
using namespace std;

struct WordInfo {
    string word;
    int score;
    vector<pair<int, int>> positions;
    bool isHorizontal;
    
    WordInfo() : score(0), isHorizontal(true) {}
};

class Player{
public:
    Player(SDL_Renderer* renderer);
    string formatTimeMMSS(Uint32 ms);
    void handle_timer(Uint32& current_time);
    void handleEvent(SDL_Event& event, int& mouseX, int& mouseY);
    void load_player_rack(const string& path);
    
    // Cập nhật các method liên quan đến tile
    void generate_letters();
    void refillRack(int playerNum);
    void exchangeTiles(int playerNum, const std::vector<int>& tileIndices);
    void repositionRackTiles();
    
    void rack_update();
    void render();
    void renderText(int x, int y, TTF_Font* font, string text);
    void set_score(int score) { this->score = score; }
    int get_score() const { return score; }
    bool isVertical(const vector<pair<int, int>>& tile_positions);
    int calculateTileScore(int x, int y, Tile* tile);
    int getTileScore(char c);
    
    // ===== MAIN VALIDATION METHOD =====
    std::pair<bool, int> canSubmitAndCalculateScore(Tile* Board[15][15], vector<pair<int, int>> tile_positions);
    
    // ===== HELPER VALIDATION METHODS =====
    bool areNewTilesStraight(Tile* Board[15][15], const vector<pair<int, int>>& tile_positions);
    bool isFirstMoveOnBoard(Tile* Board[15][15], const vector<pair<int, int>>& tile_positions);
    bool hasAdjacentConnection(Tile* Board[15][15], const vector<pair<int, int>>& tile_positions);
    bool isNewTile(int x, int y, const vector<pair<int, int>>& tile_positions);
    
    // ===== WORD FINDING METHODS =====
    vector<WordInfo> findAllWordsFormed(Tile* Board[15][15], const vector<pair<int, int>>& tile_positions);
    WordInfo findMainWord(Tile* Board[15][15], const vector<pair<int, int>>& tile_positions);
    WordInfo findCrossWord(Tile* Board[15][15], int x, int y, bool vertical, const vector<pair<int, int>>& tile_positions);
    bool areNewTilesHorizontal(const vector<pair<int, int>>& tile_positions);
    
    // ======== HINT SYSTEM METHODS ========
    void showHint(Tile* Board[15][15]);
    void renderHints();
    void clearHints();
    
    // Getter cho tile bag
    TileBag* getTileBag() { return tileBag; }
    int getRemainingTilesCount() const { return tileBag->getRemainingCount(); }

    vector<Tile> letters;
    vector<Tile> player2_letters;
    std::vector<std::pair<int, int>> tile_positions;
    SDL_Rect rack_rect;
    SDL_Rect dst_rect;
    bool is_first_player_turn = false;
    int score = 0, player1_score = 0, player2_score = 0;
    string player1_score_text = "Score: ", player2_score_text = "Score: ";
    
    // THÊM CÁC BIẾN ĐỂ TRACK VIỆC CHỌN TILE TRÊN BOARD
    bool has_selected_board_tile = false;
    int selected_board_tile_x = -1;
    int selected_board_tile_y = -1;
    
    // ======== HINT SYSTEM VARIABLES ========
    vector<HintMove> currentHints;
    bool showingHints = false;
    int selectedHintIndex = -1;
    
    ~Player();
    SDL_Texture* getRackTexture() { return rack; }

private:
    SDL_Renderer* renderer;
    SDL_Texture* rack;
    TileBag* tileBag;
    HintSystem* hintSystem;  // ======== THÊM HINT SYSTEM ========

    int letters_size = 7;
    int tile_size = 40;
    
    Uint32 player1_time_left;
    Uint32 player2_time_left;
    Uint32 time_per_match = 2000000;
    
    // Helper methods
    void fillPlayerRack(vector<Tile>& playerTiles, int startX, int startY);
    void setupTilePosition(Tile& tile, char letter, int x, int y);
};
