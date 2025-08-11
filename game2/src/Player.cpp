// Player.cpp - Fixed version with repositionRackTiles method

#include "Player.hpp"
#include <SDL_image.h>
#include "Game.hpp"
#include "Tile.hpp"
#include "UI.hpp"
#include <SDL2/SDL_ttf.h>
#include <set>
#include <cmath>
#include <string>
using namespace std;

Player::Player(SDL_Renderer* renderer) : renderer(renderer) {
    tileBag = new TileBag();
    hintSystem = new HintSystem();  // ======== KHỞI TẠO HINT SYSTEM ========
    player1_time_left = time_per_match;
    player2_time_left = time_per_match;
}

void Player::load_player_rack(const string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    rack = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void Player::setupTilePosition(Tile& tile, char letter, int x, int y) {
    tile.letter = letter;
    tile.score = tileBag->getTileScore(letter);
    tile.rect.x = x;
    tile.rect.y = y;
    tile.rect.w = tile_size;
    tile.rect.h = tile_size;
    tile.selected = false;
}

void Player::fillPlayerRack(vector<Tile>& playerTiles, int startX, int startY) {
    int tilesNeeded = letters_size - playerTiles.size();
    vector<char> newTiles = tileBag->drawTiles(tilesNeeded);
    
    for (size_t i = 0; i < newTiles.size(); i++) {
        Tile tile;
        int positionX = startX + (playerTiles.size() + i) * (tile_size - 20);
        setupTilePosition(tile, newTiles[i], positionX, startY);
        playerTiles.push_back(tile);
    }
}

void Player::generate_letters() {
    letters.clear();
    player2_letters.clear();
    
    fillPlayerRack(letters, 60, 500);
    fillPlayerRack(player2_letters, 1095, 500);
    
    SDL_Log("Generated tiles for both players. Remaining in bag: %d", tileBag->getRemainingCount());
}

// ============= FIX 2: THÊM METHOD ĐỂ REPOSITION RACK TILES =============
void Player::repositionRackTiles() {
    // Reposition Player 1 tiles
    int startX1 = 60;
    int startY1 = 500;
    int tileSpacing = tile_size; // Khoảng cách giữa các tile (20px)
    int validTileIndex = 0;
    
    for (auto& tile : letters) {
        if (tile.letter != '\0') {
            tile.rect.x = startX1 + validTileIndex * tileSpacing;
            tile.rect.y = startY1;
            tile.rect.w = tile_size;
            tile.rect.h = tile_size;
            validTileIndex++;
        }
    }
    
    // Reposition Player 2 tiles
    int startX2 = 1095;
    int startY2 = 500;
    validTileIndex = 0;
    
    for (auto& tile : player2_letters) {
        if (tile.letter != '\0') {
            tile.rect.x = startX2 + validTileIndex * tileSpacing;
            tile.rect.y = startY2;
            tile.rect.w = tile_size;
            tile.rect.h = tile_size;
            validTileIndex++;
        }
    }
    
    SDL_Log("Rack tiles repositioned with proper spacing");
}

void Player::refillRack(int playerNum) {
    vector<Tile>* targetRack = (playerNum == 1) ? &letters : &player2_letters;
    int startX = (playerNum == 1) ? 60 : 1095;
    int startY = 500;
    int tileSpacing = tile_size; // 20px spacing
    
    SDL_Log("=== REFILL RACK DEBUG - Player %d ===", playerNum);
    SDL_Log("Before refill - rack size: %zu", targetRack->size());
    
    // Đếm số tile còn lại trong rack và compact lại
    vector<Tile> remainingTiles;
    for (const auto& tile : *targetRack) {
        if (tile.letter != '\0') {
            remainingTiles.push_back(tile);
            SDL_Log("Remaining tile: %c at (%d, %d)", tile.letter, tile.rect.x, tile.rect.y);
        }
    }
    
    // Clear rack hiện tại
    targetRack->clear();
    
    // ============= FIX: ĐÚNG VỊ TRÍ KHI REPOSITION =============
    // Đặt lại các tile còn lại với vị trí đúng thứ tự
    for (size_t i = 0; i < remainingTiles.size(); i++) {
        remainingTiles[i].rect.x = startX + i * tileSpacing;
        remainingTiles[i].rect.y = startY;
        remainingTiles[i].rect.w = tile_size;
        remainingTiles[i].rect.h = tile_size;
        remainingTiles[i].selected = false; // Reset selection
        targetRack->push_back(remainingTiles[i]);
        
        SDL_Log("Repositioned tile %zu '%c' to (%d, %d)", 
               i, remainingTiles[i].letter, remainingTiles[i].rect.x, remainingTiles[i].rect.y);
    }
    
    // Lấy tile mới từ bag để fill đủ 7 tiles
    int tilesNeeded = letters_size - remainingTiles.size();
    SDL_Log("Tiles needed: %d", tilesNeeded);
    
    if (tilesNeeded > 0) {
        vector<char> newTiles = tileBag->drawTiles(tilesNeeded);
        
        for (size_t i = 0; i < newTiles.size(); i++) {
            Tile tile;
            int positionX = startX + (remainingTiles.size() + i) * tileSpacing;
            setupTilePosition(tile, newTiles[i], positionX, startY);
            targetRack->push_back(tile);
            
            SDL_Log("Added new tile %zu '%c' at (%d, %d)", 
                   remainingTiles.size() + i, newTiles[i], positionX, startY);
        }
    }
    
    SDL_Log("After refill - rack size: %zu, Remaining in bag: %d", 
            targetRack->size(), tileBag->getRemainingCount());
    SDL_Log("=== REFILL RACK COMPLETE ===");
}

void Player::exchangeTiles(int playerNum, const std::vector<int>& tileIndices) {
    if (tileBag->getRemainingCount() < tileIndices.size()) {
        SDL_Log("Not enough tiles in bag for exchange");
        return;
    }
    
    vector<Tile>* targetRack = (playerNum == 1) ? &letters : &player2_letters;
    vector<char> tilesToReturn;
    
    for (int index : tileIndices) {
        if (index >= 0 && index < targetRack->size()) {
            tilesToReturn.push_back((*targetRack)[index].letter);
            (*targetRack)[index].letter = '\0';
        }
    }
    
    tileBag->returnTiles(tilesToReturn);
    refillRack(playerNum);
    
    SDL_Log("Exchanged %zu tiles for player %d", tilesToReturn.size(), playerNum);
}

void Player::renderText(int x, int y, TTF_Font* font, string text) {
    SDL_Color color = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Player::handle_timer(Uint32& current_timer) {
    if (is_first_player_turn) {
        Uint32 player1_turn_start = SDL_GetTicks();
        player1_time_left = time_per_match - (player1_turn_start - current_timer);
        if (player1_turn_start - current_timer > time_per_match) 
            player1_time_left = 0;
    } else {
        Uint32 player2_turn_start = SDL_GetTicks();
        player2_time_left = time_per_match - (player2_turn_start);
        if (player2_turn_start - current_timer > time_per_match) 
            player2_time_left = 0;
    }
}

string Player::formatTimeMMSS(Uint32 ms) {
    Uint32 total_seconds = ms / 1000;
    Uint32 minutes = total_seconds / 60;
    Uint32 seconds = total_seconds % 60;

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes
        << ":" << std::setw(2) << std::setfill('0') << seconds;
    return oss.str();
}

void Player::render() {
    // Render player 1 frame
    SDL_Surface* frame_surface = nullptr;
    if (is_first_player_turn) {
        frame_surface = IMG_Load("frame.png");
        player1_score_text = "Score: " + to_string(player1_score);
    } else {
        frame_surface = IMG_Load("gray_frame.png");
        player1_score_text = "Score: " + to_string(player1_score);
    }

    SDL_Texture* frame_texture = SDL_CreateTextureFromSurface(renderer, frame_surface);
    SDL_Rect frame_rect = {15, 210, 370, 420};
    SDL_RenderCopy(renderer, frame_texture, nullptr, &frame_rect);
    SDL_FreeSurface(frame_surface);
    SDL_DestroyTexture(frame_texture);

    string player1_timer;
    if (player1_time_left > 0)
        player1_timer = "Time: " + formatTimeMMSS(player1_time_left);
    else
        player1_timer = "Time Over!";

    TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 24);
    renderText(160, 250, font, "Player 1");
    renderText(70, 400, font, player1_score_text);
    renderText(70, 430, font, player1_timer);
    renderText(70, 460, font, "Bag: " + to_string(tileBag->getRemainingCount()));

    // Render player 2 frame
    SDL_Surface* player2_frame_surface = nullptr;
    if (!is_first_player_turn) {
        player2_frame_surface = IMG_Load("frame.png");
        player2_score_text = "Score: " + to_string(player2_score);
    } else {
        player2_frame_surface = IMG_Load("gray_frame.png");
        player2_score_text = "Score: " + to_string(player2_score);
    }

    if (!player2_frame_surface) {
        std::cerr << "Failed to load frame: " << IMG_GetError() << std::endl;
    }

    SDL_Texture* player2_frame_texture = SDL_CreateTextureFromSurface(renderer, player2_frame_surface);
    SDL_Rect player2_frame_rect = {1050, 210, 370, 420};
    SDL_RenderCopy(renderer, player2_frame_texture, nullptr, &player2_frame_rect);
    SDL_FreeSurface(player2_frame_surface);
    SDL_DestroyTexture(player2_frame_texture);

    string timer;
    if (player2_time_left > 0)
        timer = "Time: " + formatTimeMMSS(player2_time_left);
    else
        timer = "Time Over!";

    // Render player 2 text
    renderText(1200, 250, font, "Player 2");
    renderText(1105, 400, font, player2_score_text);
    renderText(1105, 430, font, timer);

    TTF_CloseFont(font);

    // Render player's racks
    for (size_t i = 0; i < letters.size(); i++) {
        if (letters[i].letter != '\0') {
            int index = letters[i].letter - 'A';
            if (index < 0 || index >= 26) index = 0;

            SDL_Rect rack_rect = {letters[i].rect.x, letters[i].rect.y, tile_size, tile_size};
            SDL_Rect dst_rect = {index * tile_size, 0, tile_size, tile_size};

            letters[i].rect = rack_rect;
            SDL_RenderCopy(renderer, rack, &dst_rect, &rack_rect);
        }
    }

    for (size_t i = 0; i < player2_letters.size(); i++) {
        if (player2_letters[i].letter != '\0') {
            int index = player2_letters[i].letter - 'A';
            if (index < 0 || index >= 26) index = 0;

            SDL_Rect rack_rect = {player2_letters[i].rect.x, player2_letters[i].rect.y, tile_size, tile_size};
            SDL_Rect dst_rect = {index * tile_size, 0, tile_size, tile_size};

            player2_letters[i].rect = rack_rect;
            SDL_RenderCopy(renderer, rack, &dst_rect, &rack_rect);
        }
    }
}

bool Player::isVertical(const vector<pair<int, int>>& tile_positions) {
    int first_x = tile_positions[0].first;
    for (const auto& pos : tile_positions) {
        if (pos.first != first_x) {
            return false;
        }
    }
    return true;
}

int Player::calculateTileScore(int x, int y, Tile* tile) {
    int tileScore = tile->score;
    BonusType bonus = board_bonus[x][y];
    switch (bonus) {
        case DOUBLE_LETTER:
            tileScore *= 2;
            break;
        case TRIPLE_LETTER:
            tileScore *= 3;
            break;
        default:
            break;
    }
    return tileScore;
}

int Player::getTileScore(char c) {
    return tileBag->getTileScore(c);
}



std::pair<bool, int> Player::canSubmitAndCalculateScore(Tile* Board[15][15], vector<pair<int, int>> tile_positions) {
    SDL_Log("=== COMPLETE SCRABBLE VALIDATION ===");
    SDL_Log("Input tile_positions size: %zu", tile_positions.size());
    
    if (tile_positions.empty()) {
        SDL_Log("No tile positions provided");
        return {false, 0};
    }
    
    // ===== 1. KIỂM TRA TILE MỚI PHẢI THẲNG HÀNG =====
    if (!areNewTilesStraight(Board, tile_positions)) {
        SDL_Log("FAIL: New tiles are not in straight line or have gaps");
        return {false, 0};
    }
    
    // ===== 2. KIỂM TRA LƯỢT ĐẦU TIÊN =====
    bool isFirstMove = isFirstMoveOnBoard(Board, tile_positions);
    SDL_Log("Is first move: %s", isFirstMove ? "YES" : "NO");
    
    if (isFirstMove) {
        bool hasCenterTile = false;
        for (const auto& pos : tile_positions) {
            if (pos.first == 7 && pos.second == 7) {
                hasCenterTile = true;
                break;
            }
        }
        if (!hasCenterTile) {
            SDL_Log("FAIL: First move must pass through center (7,7)");
            return {false, 0};
        }
        SDL_Log("PASS: First move goes through center");
    }
    
    // ===== 3. KIỂM TRA KẾT NỐI VỚI TILE CŨ =====
    if (!isFirstMove && !hasAdjacentConnection(Board, tile_positions)) {
        SDL_Log("FAIL: New tiles must connect to existing tiles");
        return {false, 0};
    }
    
    // ===== 4. TÌM TẤT CẢ CÁC TỪ ĐƯỢC TẠO RA =====
    vector<WordInfo> wordsFormed = findAllWordsFormed(Board, tile_positions);
    
    if (wordsFormed.empty()) {
        SDL_Log("FAIL: No words formed");
        return {false, 0};
    }
    
    SDL_Log("Found %zu words:", wordsFormed.size());
    
    // ===== 5. VALIDATE TẤT CẢ TỪ VỚI DICTIONARY =====
    Graph* dictionary = Graph::getInstance();
    int total_score = 0;
    
    for (const auto& wordInfo : wordsFormed) {
        SDL_Log("Checking word: '%s' (length: %d)", wordInfo.word.c_str(), (int)wordInfo.word.length());
        
        if (wordInfo.word.length() < 2) {
            SDL_Log("SKIP: Single letter word");
            continue; // Skip single letters
        }
        
        if (!dictionary->isWordInDictionary(wordInfo.word)) {
            SDL_Log("FAIL: Invalid word '%s' not in dictionary", wordInfo.word.c_str());
            return {false, 0};
        }
        
        SDL_Log("VALID: '%s' (score: %d)", wordInfo.word.c_str(), wordInfo.score);
        total_score += wordInfo.score;
    }
    
    // ===== 6. BONUS 50 ĐIỂM NẾU DÙNG HẾT 7 TILES =====
    if (tile_positions.size() == 7) {
        total_score += 50;
        SDL_Log("BINGO BONUS: +50 points for using all 7 tiles");
    }
    
    SDL_Log("=== VALIDATION SUCCESSFUL ===");
    SDL_Log("Total score: %d", total_score);
    return {true, total_score};
}

bool Player::areNewTilesStraight(Tile* Board[15][15], const vector<pair<int, int>>& tile_positions) {
    if (tile_positions.size() <= 1) return true;
    
    // Sort positions để kiểm tra
    vector<pair<int, int>> sortedPos = tile_positions;
    std::sort(sortedPos.begin(), sortedPos.end());
    
    // Check if all tiles are in same row
    bool sameRow = true;
    int row = sortedPos[0].second;
    for (const auto& pos : sortedPos) {
        if (pos.second != row) {
            sameRow = false;
            break;
        }
    }
    
    // Check if all tiles are in same column  
    bool sameCol = true;
    int col = sortedPos[0].first;
    for (const auto& pos : sortedPos) {
        if (pos.first != col) {
            sameCol = false;
            break;
        }
    }
    
    if (!sameRow && !sameCol) {
        SDL_Log("Tiles not in same row or column");
        return false;
    }
    
    // Check for gaps - tiles must be connected (allowing existing tiles in between)
    if (sameRow) {
        // Sort by column for row check
        std::sort(sortedPos.begin(), sortedPos.end(), 
            [](const pair<int,int>& a, const pair<int,int>& b) {
                return a.first < b.first;
            });
            
        for (size_t i = 1; i < sortedPos.size(); i++) {
            int gap = sortedPos[i].first - sortedPos[i-1].first;
            if (gap > 1) {
                // Check if gap is filled by existing tiles
                for (int x = sortedPos[i-1].first + 1; x < sortedPos[i].first; x++) {
                    if (Board[x][row] == nullptr) {
                        SDL_Log("Gap found at (%d,%d) in horizontal word", x, row);
                        return false;
                    }
                }
            }
        }
    } else { // sameCol
        // Sort by row for column check
        std::sort(sortedPos.begin(), sortedPos.end(), 
            [](const pair<int,int>& a, const pair<int,int>& b) {
                return a.second < b.second;
            });
            
        for (size_t i = 1; i < sortedPos.size(); i++) {
            int gap = sortedPos[i].second - sortedPos[i-1].second;
            if (gap > 1) {
                // Check if gap is filled by existing tiles
                for (int y = sortedPos[i-1].second + 1; y < sortedPos[i].second; y++) {
                    if (Board[col][y] == nullptr) {
                        SDL_Log("Gap found at (%d,%d) in vertical word", col, y);
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
}

bool Player::isFirstMoveOnBoard(Tile* Board[15][15], const vector<pair<int, int>>& tile_positions) {
    // Check if there are any existing tiles on board (not in current move)
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            if (Board[i][j] != nullptr) {
                // Check if this tile is part of current move
                bool isNewTile = false;
                for (const auto& pos : tile_positions) {
                    if (pos.first == i && pos.second == j) {
                        isNewTile = true;
                        break;
                    }
                }
                if (!isNewTile) {
                    return false; // Found existing tile not from current move
                }
            }
        }
    }
    return true; // Only new tiles found on board
}

bool Player::hasAdjacentConnection(Tile* Board[15][15], const vector<pair<int, int>>& tile_positions) {
    for (const auto& pos : tile_positions) {
        int x = pos.first, y = pos.second;
        
        // Check 4 directions for existing tiles (not from current move)
        int dx[] = {-1, 1, 0, 0};
        int dy[] = {0, 0, -1, 1};
        
        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            
            if (nx >= 0 && nx < 15 && ny >= 0 && ny < 15) {
                if (Board[nx][ny] != nullptr && !isNewTile(nx, ny, tile_positions)) {
                    return true; // Found adjacent existing tile
                }
            }
        }
    }
    return false;
}

bool Player::isNewTile(int x, int y, const vector<pair<int, int>>& tile_positions) {
    for (const auto& pos : tile_positions) {
        if (pos.first == x && pos.second == y) {
            return true;
        }
    }
    return false;
}

vector<WordInfo> Player::findAllWordsFormed(Tile* Board[15][15], const vector<pair<int, int>>& tile_positions) {
    vector<WordInfo> allWords;
    set<string> uniqueWords; // Avoid duplicates
    
    // ===== FIND MAIN WORD =====
    WordInfo mainWord = findMainWord(Board, tile_positions);
    if (mainWord.word.length() > 1) {
        allWords.push_back(mainWord);
        uniqueWords.insert(mainWord.word);
        SDL_Log("Main word: '%s' (score: %d)", mainWord.word.c_str(), mainWord.score);
    }
    
    // ===== FIND ALL CROSSWORDS =====
    for (const auto& pos : tile_positions) {
        // Find vertical crossword
        WordInfo verticalWord = findCrossWord(Board, pos.first, pos.second, true, tile_positions);
        if (verticalWord.word.length() > 1 && uniqueWords.find(verticalWord.word) == uniqueWords.end()) {
            allWords.push_back(verticalWord);
            uniqueWords.insert(verticalWord.word);
            SDL_Log("Vertical crossword: '%s' (score: %d)", verticalWord.word.c_str(), verticalWord.score);
        }
        
        // Find horizontal crossword
        WordInfo horizontalWord = findCrossWord(Board, pos.first, pos.second, false, tile_positions);
        if (horizontalWord.word.length() > 1 && uniqueWords.find(horizontalWord.word) == uniqueWords.end()) {
            allWords.push_back(horizontalWord);
            uniqueWords.insert(horizontalWord.word);
            SDL_Log("Horizontal crossword: '%s' (score: %d)", horizontalWord.word.c_str(), horizontalWord.score);
        }
    }
    
    return allWords;
}

WordInfo Player::findMainWord(Tile* Board[15][15], const vector<pair<int, int>>& tile_positions) {
    WordInfo wordInfo;
    
    if (tile_positions.empty()) return wordInfo;
    
    // Determine if new tiles form horizontal or vertical word
    bool isHorizontal = areNewTilesHorizontal(tile_positions);
    wordInfo.isHorizontal = isHorizontal;
    
    if (isHorizontal) {
        int row = tile_positions[0].second;
        int minCol = tile_positions[0].first;
        int maxCol = tile_positions[0].first;
        
        // Find bounds of new tiles
        for (const auto& pos : tile_positions) {
            minCol = min(minCol, pos.first);
            maxCol = max(maxCol, pos.first);
        }
        
        // Expand to include adjacent tiles
        while (minCol > 0 && Board[minCol-1][row] != nullptr) minCol--;
        while (maxCol < 14 && Board[maxCol+1][row] != nullptr) maxCol++;
        
        // Build word and calculate score
        int wordMultiplier = 1;
        for (int x = minCol; x <= maxCol; x++) {
            if (Board[x][row] != nullptr) {
                wordInfo.word += Board[x][row]->letter;
                wordInfo.positions.push_back({x, row});
                
                // Calculate tile score
                int tileScore = Board[x][row]->score;
                bool isNewTileHere = isNewTile(x, row, tile_positions);
                
                if (isNewTileHere) {
                    // Apply letter multipliers for new tiles
                    BonusType bonus = board_bonus[x][row];
                    if (bonus == DOUBLE_LETTER) tileScore *= 2;
                    else if (bonus == TRIPLE_LETTER) tileScore *= 3;
                    else if (bonus == DOUBLE_WORD) wordMultiplier *= 2;
                    else if (bonus == TRIPLE_WORD) wordMultiplier *= 3;
                }
                
                wordInfo.score += tileScore;
            }
        }
        
        wordInfo.score *= wordMultiplier;
        
    } else { // Vertical word
        int col = tile_positions[0].first;
        int minRow = tile_positions[0].second;
        int maxRow = tile_positions[0].second;
        
        // Find bounds of new tiles
        for (const auto& pos : tile_positions) {
            minRow = min(minRow, pos.second);
            maxRow = max(maxRow, pos.second);
        }
        
        // Expand to include adjacent tiles
        while (minRow > 0 && Board[col][minRow-1] != nullptr) minRow--;
        while (maxRow < 14 && Board[col][maxRow+1] != nullptr) maxRow++;
        
        // Build word and calculate score
        int wordMultiplier = 1;
        for (int y = minRow; y <= maxRow; y++) {
            if (Board[col][y] != nullptr) {
                wordInfo.word += Board[col][y]->letter;
                wordInfo.positions.push_back({col, y});
                
                // Calculate tile score
                int tileScore = Board[col][y]->score;
                bool isNewTileHere = isNewTile(col, y, tile_positions);
                
                if (isNewTileHere) {
                    // Apply letter multipliers for new tiles
                    BonusType bonus = board_bonus[col][y];
                    if (bonus == DOUBLE_LETTER) tileScore *= 2;
                    else if (bonus == TRIPLE_LETTER) tileScore *= 3;
                    else if (bonus == DOUBLE_WORD) wordMultiplier *= 2;
                    else if (bonus == TRIPLE_WORD) wordMultiplier *= 3;
                }
                
                wordInfo.score += tileScore;
            }
        }
        
        wordInfo.score *= wordMultiplier;
    }
    
    return wordInfo;
}

WordInfo Player::findCrossWord(Tile* Board[15][15], int x, int y, bool vertical, const vector<pair<int, int>>& tile_positions) {
    WordInfo wordInfo;
    
    if (vertical) {
        // Find vertical word containing (x,y)
        int minRow = y, maxRow = y;
        
        // Expand up and down
        while (minRow > 0 && Board[x][minRow-1] != nullptr) minRow--;
        while (maxRow < 14 && Board[x][maxRow+1] != nullptr) maxRow++;
        
        // Only count if word is longer than 1 letter
        if (maxRow > minRow) {
            int wordMultiplier = 1;
            bool hasNewTile = false;
            
            for (int row = minRow; row <= maxRow; row++) {
                if (Board[x][row] != nullptr) {
                    wordInfo.word += Board[x][row]->letter;
                    wordInfo.positions.push_back({x, row});
                    
                    int tileScore = Board[x][row]->score;
                    bool isNewTileHere = isNewTile(x, row, tile_positions);
                    
                    if (isNewTileHere) {
                        hasNewTile = true;
                        BonusType bonus = board_bonus[x][row];
                        if (bonus == DOUBLE_LETTER) tileScore *= 2;
                        else if (bonus == TRIPLE_LETTER) tileScore *= 3;
                        else if (bonus == DOUBLE_WORD) wordMultiplier *= 2;
                        else if (bonus == TRIPLE_WORD) wordMultiplier *= 3;
                    }
                    
                    wordInfo.score += tileScore;
                }
            }
            
            // Only count crosswords that contain at least one new tile
            if (!hasNewTile) {
                wordInfo.word.clear();
                wordInfo.score = 0;
                wordInfo.positions.clear();
            } else {
                wordInfo.score *= wordMultiplier;
                wordInfo.isHorizontal = false;
            }
        }
        
    } else {
        // Find horizontal word containing (x,y)
        int minCol = x, maxCol = x;
        
        // Expand left and right
        while (minCol > 0 && Board[minCol-1][y] != nullptr) minCol--;
        while (maxCol < 14 && Board[maxCol+1][y] != nullptr) maxCol++;
        
        // Only count if word is longer than 1 letter
        if (maxCol > minCol) {
            int wordMultiplier = 1;
            bool hasNewTile = false;
            
            for (int col = minCol; col <= maxCol; col++) {
                if (Board[col][y] != nullptr) {
                    wordInfo.word += Board[col][y]->letter;
                    wordInfo.positions.push_back({col, y});
                    
                    int tileScore = Board[col][y]->score;
                    bool isNewTileHere = isNewTile(col, y, tile_positions);
                    
                    if (isNewTileHere) {
                        hasNewTile = true;
                        BonusType bonus = board_bonus[col][y];
                        if (bonus == DOUBLE_LETTER) tileScore *= 2;
                        else if (bonus == TRIPLE_LETTER) tileScore *= 3;
                        else if (bonus == DOUBLE_WORD) wordMultiplier *= 2;
                        else if (bonus == TRIPLE_WORD) wordMultiplier *= 3;
                    }
                    
                    wordInfo.score += tileScore;
                }
            }
            
            // Only count crosswords that contain at least one new tile
            if (!hasNewTile) {
                wordInfo.word.clear();
                wordInfo.score = 0;
                wordInfo.positions.clear();
            } else {
                wordInfo.score *= wordMultiplier;
                wordInfo.isHorizontal = true;
            }
        }
    }
    
    return wordInfo;
}

bool Player::areNewTilesHorizontal(const vector<pair<int, int>>& tile_positions) {
    if (tile_positions.size() <= 1) return true;
    
    int row = tile_positions[0].second;
    for (const auto& pos : tile_positions) {
        if (pos.second != row) return false;
    }
    return true;
}

void Player::handleEvent(SDL_Event& event, int& mouseX, int& mouseY) {
    UI ui(Player::renderer);
    SDL_Point mousePoint = {mouseX, mouseY};
}

void Player::showHint(Tile* Board[15][15]) {
    SDL_Log("=== GENERATING HINTS ===");
    
    // Clear hints cũ
    clearHints();
    
    // Lấy rack của player hiện tại
    vector<Tile>* currentRack = is_first_player_turn ? &letters : &player2_letters;
    
    // Tạo hints
    currentHints = hintSystem->generateHints(Board, *currentRack, 5);
    showingHints = true;
    selectedHintIndex = -1;
    
    SDL_Log("Generated %zu hints for %s", currentHints.size(), 
            is_first_player_turn ? "Player 1" : "Player 2");
    
    // Log các hints tìm được
    for (size_t i = 0; i < currentHints.size(); i++) {
        const HintMove& hint = currentHints[i];
        SDL_Log("Hint %zu: '%s' at (%d,%d) %s - Score: %d", 
                i + 1, hint.word.c_str(), hint.startX, hint.startY,
                hint.isHorizontal ? "Horizontal" : "Vertical", hint.score);
    }
}

void Player::renderHints() {
    if (!showingHints || currentHints.empty()) return;
    
    TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 18);
    if (!font) return;
    
    // Vẽ background cho hint panel
    SDL_Rect hintPanel = {50, 100, 300, 200};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, &hintPanel);
    
    // Vẽ viền
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_RenderDrawRect(renderer, &hintPanel);
    
    // Tiêu đề
    SDL_Color white = {255, 255, 255};
    SDL_Surface* titleSurface = TTF_RenderText_Blended(font, "HINTS:", white);
    if (titleSurface) {
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
        SDL_Rect titleRect = {hintPanel.x + 10, hintPanel.y + 10, titleSurface->w, titleSurface->h};
        SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
        SDL_FreeSurface(titleSurface);
        SDL_DestroyTexture(titleTexture);
    }
    
    // Render từng hint
    int yOffset = hintPanel.y + 40;
    for (size_t i = 0; i < currentHints.size() && i < 5; i++) {
        const HintMove& hint = currentHints[i];
        
        // Highlight hint được chọn
        SDL_Color textColor = (i == selectedHintIndex) ? 
            SDL_Color{255, 255, 0} : SDL_Color{255, 255, 255};
        
        // Tạo text cho hint
        string hintText = to_string(i + 1) + ". " + hint.word + " (" + to_string(hint.score) + " pts)";
        
        SDL_Surface* hintSurface = TTF_RenderText_Blended(font, hintText.c_str(), textColor);
        if (hintSurface) {
            SDL_Texture* hintTexture = SDL_CreateTextureFromSurface(renderer, hintSurface);
            SDL_Rect hintRect = {hintPanel.x + 10, yOffset, hintSurface->w, hintSurface->h};
            SDL_RenderCopy(renderer, hintTexture, nullptr, &hintRect);
            SDL_FreeSurface(hintSurface);
            SDL_DestroyTexture(hintTexture);
        }
        
        yOffset += 25;
    }
    
    // Hướng dẫn sử dụng
    SDL_Color grayColor = {180, 180, 180};
    SDL_Surface* instructionSurface = TTF_RenderText_Blended(font, "Click number to select", grayColor);
    if (instructionSurface) {
        SDL_Texture* instructionTexture = SDL_CreateTextureFromSurface(renderer, instructionSurface);
        SDL_Rect instructionRect = {hintPanel.x + 10, hintPanel.y + 170, instructionSurface->w, instructionSurface->h};
        SDL_RenderCopy(renderer, instructionTexture, nullptr, &instructionRect);
        SDL_FreeSurface(instructionSurface);
        SDL_DestroyTexture(instructionTexture);
    }
    
    TTF_CloseFont(font);
}

void Player::clearHints() {
    currentHints.clear();
    showingHints = false;
    selectedHintIndex = -1;
    SDL_Log("Hints cleared");
}

// ======== CẬP NHẬT DESTRUCTOR ========
Player::~Player() {
    if (tileBag) {
        delete tileBag;
        tileBag = nullptr;
    }
    
    if (hintSystem) {  // ======== THÊM CLEANUP CHO HINT SYSTEM ========
        delete hintSystem;
        hintSystem = nullptr;
    }
    
    if (rack) {
        SDL_DestroyTexture(rack);
        rack = nullptr;
    }
}