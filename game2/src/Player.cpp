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
    Graph* graph = Graph::getInstance();
    int total_score = 0;
    bool allWordsValid = true;
    std::set<std::string> validWords; // Dùng set để tránh trùng từ
    std::set<std::pair<int, int>> newTiles(tile_positions.begin(), tile_positions.end());

    bool touchesExisting = false;
    for (auto& pos : tile_positions) {
        int x = pos.first;
        int y = pos.second;
        const int dx[4] = {-1, 1, 0, 0};
        const int dy[4] = {0, 0, -1, 1};
        for (int k = 0; k < 4; ++k) {
            int nx = x + dx[k], ny = y + dy[k];
            if (nx >= 0 && nx < 15 && ny >= 0 && ny < 15) {
                // phải là ô cũ (không nằm trong newTiles)
                if (Board[nx][ny] != nullptr && newTiles.count({nx, ny}) == 0) {
                    touchesExisting = true;
                    break;
                }
            }
        }
    }

    bool isFirstMove = true;
    for (int i = 0; i < 15 && isFirstMove; i++) {
        for (int j = 0; j < 15 && isFirstMove; j++) {
            if (Board[i][j] != nullptr && newTiles.count({i, j}) == 0) {
                isFirstMove = false; // tìm thấy ô cũ (không phải ô mới đặt)
            }
        }
    }

    if (isFirstMove) {
        if (newTiles.count({7,7}) == 0) {
            return {false, 0}; 
        }
    } else {
        if (!touchesExisting) return {false, 0}; // lượt sau phải chạm ô cũ
    }

    bool vertical = isVertical(tile_positions);

    std::string mainWord;
    int mainScore = 0;
    int doubleWordCount = 0, tripleWordCount = 0;


    if (!vertical) {
        int y = tile_positions[0].second;
        int minX = 15, maxX = -1;
        for (auto& pos : tile_positions) {
            minX = std::min(minX, pos.first);
            maxX = std::max(maxX, pos.first);
        }
        for (int x = minX; x <= maxX; ++x) {
        if (Board[x][y] == nullptr) {
            return {false, 0};
        }
    }
        int x = minX;
        while (x > 0 && Board[x - 1][y] != nullptr) x--;
        for (; x < 15 && Board[x][y] != nullptr; ++x) {
            mainWord += Board[x][y]->letter;
            int tileScore = getTileScore(Board[x][y]->letter);
            if (newTiles.count({x, y})) {
                switch (board_bonus[x][y]) {
                    case DOUBLE_LETTER: tileScore *= 2; break;
                    case TRIPLE_LETTER: tileScore *= 3; break;
                    case DOUBLE_WORD: doubleWordCount++; break;
                    case TRIPLE_WORD: tripleWordCount++; break;
                    default: break;
                }
            }
            mainScore += tileScore;
        }
    } else {
        int x = tile_positions[0].first;
        int minY = 15, maxY = -1;
        for (auto& pos : tile_positions) {
            minY = std::min(minY, pos.second);
            maxY = std::max(maxY, pos.second);
        }
        for (int y = minY; y <= maxY; ++y) {
        if (Board[x][y] == nullptr) {
            return {false, 0};
        }
    }
        int y = minY;
        while (y > 0 && Board[x][y - 1] != nullptr) y--;
        for (; y < 15 && Board[x][y] != nullptr; ++y) {
            mainWord += Board[x][y]->letter;
            int tileScore = getTileScore(Board[x][y]->letter);
            if (newTiles.count({x, y})) {
                switch (board_bonus[x][y]) {
                    case DOUBLE_LETTER: tileScore *= 2; break;
                    case TRIPLE_LETTER: tileScore *= 3; break;
                    case DOUBLE_WORD: doubleWordCount++; break;
                    case TRIPLE_WORD: tripleWordCount++; break;
                    default: break;
                }
            }
            mainScore += tileScore;
        }
    }
    if(mainWord.length() >= 2) {    
        if (mainWord.empty() || !graph->isWordInDictionary(mainWord)) {
            allWordsValid = false;
        } else validWords.insert(mainWord);
    }
    

    mainScore *= std::pow(2, doubleWordCount) * std::pow(3, tripleWordCount);
    total_score += mainScore;

    for (auto& pos : tile_positions) {
        int x = pos.first, y = pos.second;
        std::string crossWord;
        int crossScore = 0;
        int crossDoubleWord = 0, crossTripleWord = 0;
        if (!vertical) {
            int left = y, right = y;
            while (left > 0 && Board[x][left - 1] != nullptr) left--;
            while (right < 14 && Board[x][right + 1] != nullptr) right++;
            if (left != right) {
                for (int j = left; j <= right; ++j) {
                    crossWord += Board[x][j]->letter;
                    int tileScore = getTileScore(Board[x][j]->letter);
                    if (j == y) {
                        switch (board_bonus[x][j]) {
                            case DOUBLE_LETTER: tileScore *= 2; break;
                            case TRIPLE_LETTER: tileScore *= 3; break;
                            case DOUBLE_WORD: crossDoubleWord++; break;
                            case TRIPLE_WORD: crossTripleWord++; break;
                            default: break;
                        }
                    }
                    crossScore += tileScore;
                }

                if (crossWord.length() >= 2) {
                    if (graph->isWordInDictionary(crossWord)) {
                        if (validWords.count(crossWord) == 0) {
                            crossScore *= std::pow(2, crossDoubleWord) * std::pow(3, crossTripleWord);
                            total_score += crossScore;
                            validWords.insert(crossWord);
                        }
                    } else {
                         allWordsValid = false;
                    }
                }
            }
        } else {
            int up = x, down = x;
            while (up > 0 && Board[up - 1][y] != nullptr) up--;
            while (down < 14 && Board[down + 1][y] != nullptr) down++;
            if (up != down) {
                for (int i = up; i <= down; ++i) {
                    crossWord += Board[i][y]->letter;
                    int tileScore = getTileScore(Board[i][y]->letter);
                    if (i == x) {
                        switch (board_bonus[i][y]) {
                            case DOUBLE_LETTER: tileScore *= 2; break;
                            case TRIPLE_LETTER: tileScore *= 3; break;
                            case DOUBLE_WORD: crossDoubleWord++; break;
                            case TRIPLE_WORD: crossTripleWord++; break;
                            default: break;
                        }
                    }
                    crossScore += tileScore;

                }
                if (crossWord.length() >= 2) {
                    if (graph->isWordInDictionary(crossWord)) {
                        if (validWords.count(crossWord) == 0) {
                            crossScore *= std::pow(2, crossDoubleWord) * std::pow(3, crossTripleWord);
                            total_score += crossScore;
                            validWords.insert(crossWord);
                        }
                    } else {
                        allWordsValid = false;
                    }
                }
            }
        }
    }

    if (tile_positions.size() == 7) total_score += 50;
    return {allWordsValid, total_score};
}

void Player::handleEvent(SDL_Event& event, int& mouseX, int& mouseY) {
    UI ui(Player::renderer);
    SDL_Point mousePoint = {mouseX, mouseY};
}

Player::~Player() {
    if (tileBag) {
        delete tileBag;
        tileBag = nullptr;
    }
    
    if (rack) {
        SDL_DestroyTexture(rack);
        rack = nullptr;
    }
}