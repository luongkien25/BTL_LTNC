#include "HintSystem.hpp"
#include "TileBag.hpp"
#include <algorithm>
#include <iostream>
#include <queue>
#include <functional> // Để sử dụng std::function

HintSystem::HintSystem() {
    dictionary = Graph::getInstance();
}

vector<HintMove> HintSystem::generateHints(Tile* Board[15][15], const vector<Tile>& playerRack, int maxHints) {
    vector<HintMove> hints;
    vector<char> availableLetters;
    
    // Lấy các chữ cái từ rack của người chơi
    for (const auto& tile : playerRack) {
        if (tile.letter != '\0') {
            availableLetters.push_back(tile.letter);
        }
    }
    
    if (availableLetters.empty()) {
        SDL_Log("No tiles in rack for hints");
        return hints;
    }
    
    // Kiểm tra xem có phải lượt đầu tiên không
    bool isFirstMove = true;
    for (int i = 0; i < 15 && isFirstMove; i++) {
        for (int j = 0; j < 15 && isFirstMove; j++) {
            if (Board[i][j] != nullptr) {
                isFirstMove = false;
            }
        }
    }
    
    if (isFirstMove) {
        // Nước đi đầu tiên - tìm từ qua center (7,7)
        generateFirstMoveHints(Board, availableLetters, hints);
    } else {
        // Tìm anchor positions
        vector<pair<int, int>> anchors = findAnchorPositions(Board);
        
        // Thử tạo từ tại mỗi anchor position
        for (const auto& anchor : anchors) {
            generateHintsAtPosition(Board, availableLetters, anchor.first, anchor.second, hints);
        }
    }
    
    // Sắp xếp hints theo điểm số giảm dần
    sort(hints.begin(), hints.end(), [](const HintMove& a, const HintMove& b) {
        return a.score > b.score;
    });
    
    // Giới hạn số lượng hints
    if (hints.size() > maxHints) {
        hints.resize(maxHints);
    }
    
    SDL_Log("Generated %zu hints", hints.size());
    return hints;
}

void HintSystem::generateFirstMoveHints(Tile* Board[15][15], const vector<char>& availableLetters, vector<HintMove>& hints) {
    // Tạo tất cả hoán vị có thể
    vector<string> permutations = generatePermutations(availableLetters, 7);
    
    for (const string& word : permutations) {
        if (word.length() < 2 || !dictionary->isWordInDictionary(word)) continue;
        
        // Thử đặt từ horizontal qua center
        int startX = 7 - (word.length() / 2);
        if (startX >= 0 && startX + word.length() <= 15) {
            HintMove move;
            if (canPlaceWordAt(Board, word, startX, 7, true, availableLetters, move)) {
                move.word = word;
                move.startX = startX;
                move.startY = 7;
                move.isHorizontal = true;
                move.score = calculateWordScore(Board, word, startX, 7, true, move.positions);
                hints.push_back(move);
            }
        }
        
        // Thử đặt từ vertical qua center
        int startY = 7 - (word.length() / 2);
        if (startY >= 0 && startY + word.length() <= 15) {
            HintMove move;
            if (canPlaceWordAt(Board, word, 7, startY, false, availableLetters, move)) {
                move.word = word;
                move.startX = 7;
                move.startY = startY;
                move.isHorizontal = false;
                move.score = calculateWordScore(Board, word, 7, startY, false, move.positions);
                hints.push_back(move);
            }
        }
    }
}

void HintSystem::generateHintsAtPosition(Tile* Board[15][15], const vector<char>& availableLetters, 
                                       int anchorX, int anchorY, vector<HintMove>& hints) {
    // Tạo hoán vị từ rack
    vector<string> permutations = generatePermutations(availableLetters, 7);
    
    for (const string& word : permutations) {
        if (word.length() < 2 || !dictionary->isWordInDictionary(word)) continue;
        
        // Thử các vị trí khác nhau để đặt từ
        // Horizontal placements
        for (int startOffset = 0; startOffset < (int)word.length(); startOffset++) {
            int startX = anchorX - startOffset;
            if (startX >= 0 && startX + (int)word.length() <= 15) {
                HintMove move;
                if (canPlaceWordAt(Board, word, startX, anchorY, true, availableLetters, move)) {
                    // Kiểm tra xem có kết nối với anchor không
                    bool connectsToAnchor = false;
                    for (const auto& pos : move.positions) {
                        if (pos.first == anchorX && pos.second == anchorY) {
                            connectsToAnchor = true;
                            break;
                        }
                    }
                    
                    if (connectsToAnchor || hasAdjacentTile(Board, startX, anchorY)) {
                        move.word = word;
                        move.startX = startX;
                        move.startY = anchorY;
                        move.isHorizontal = true;
                        move.score = calculateWordScore(Board, word, startX, anchorY, true, move.positions);
                        hints.push_back(move);
                    }
                }
            }
        }
        
        // Vertical placements
        for (int startOffset = 0; startOffset < (int)word.length(); startOffset++) {
            int startY = anchorY - startOffset;
            if (startY >= 0 && startY + (int)word.length() <= 15) {
                HintMove move;
                if (canPlaceWordAt(Board, word, anchorX, startY, false, availableLetters, move)) {
                    // Kiểm tra kết nối
                    bool connectsToAnchor = false;
                    for (const auto& pos : move.positions) {
                        if (pos.first == anchorX && pos.second == anchorY) {
                            connectsToAnchor = true;
                            break;
                        }
                    }
                    
                    if (connectsToAnchor || hasAdjacentTile(Board, anchorX, startY)) {
                        move.word = word;
                        move.startX = anchorX;
                        move.startY = startY;
                        move.isHorizontal = false;
                        move.score = calculateWordScore(Board, word, anchorX, startY, false, move.positions);
                        hints.push_back(move);
                    }
                }
            }
        }
    }
}

vector<pair<int, int>> HintSystem::findAnchorPositions(Tile* Board[15][15]) {
    vector<pair<int, int>> anchors;
    
    for (int x = 0; x < 15; x++) {
        for (int y = 0; y < 15; y++) {
            if (Board[x][y] == nullptr) { // Ô trống
                // Kiểm tra có tile kế bên không
                if (hasAdjacentTile(Board, x, y)) {
                    anchors.push_back({x, y});
                }
            }
        }
    }
    
    return anchors;
}

vector<string> HintSystem::generatePermutations(const vector<char>& letters, int maxLength) {
    set<string> uniqueWords;
    vector<string> result;
    
    // Tạo tất cả substring và hoán vị (sử dụng lambda function đúng cách)
    std::function<void(string, vector<char>)> generateRecursive;
    generateRecursive = [&](string current, vector<char> remaining) {
        if (current.length() >= 2 && current.length() <= maxLength) {
            if (dictionary->isWordInDictionary(current)) {
                uniqueWords.insert(current);
            }
        }
        
        if (current.length() >= maxLength || remaining.empty()) return;
        
        for (size_t i = 0; i < remaining.size(); i++) {
            string newCurrent = current + remaining[i];
            vector<char> newRemaining = remaining;
            newRemaining.erase(newRemaining.begin() + i);
            generateRecursive(newCurrent, newRemaining);
        }
    };
    
    generateRecursive("", letters);
    
    for (const string& word : uniqueWords) {
        result.push_back(word);
    }
    
    return result;
}

bool HintSystem::canPlaceWordAt(Tile* Board[15][15], const string& word, int x, int y, bool horizontal,
                               const vector<char>& availableLetters, HintMove& move) {
    vector<char> lettersNeeded;
    move.positions.clear();
    
    for (size_t i = 0; i < word.length(); i++) {
        int posX = horizontal ? x + i : x;
        int posY = horizontal ? y : y + i;
        
        if (posX >= 15 || posY >= 15) return false;
        
        if (Board[posX][posY] != nullptr) {
            // Phải match với tile hiện có
            if (Board[posX][posY]->letter != word[i]) {
                return false;
            }
        } else {
            // Cần tile mới từ rack
            lettersNeeded.push_back(word[i]);
            move.positions.push_back({posX, posY});
        }
    }
    
    // Kiểm tra có đủ tile trong rack không
    vector<char> rackCopy = availableLetters;
    for (char needed : lettersNeeded) {
        auto it = find(rackCopy.begin(), rackCopy.end(), needed);
        if (it == rackCopy.end()) {
            return false; // Không có tile cần thiết
        }
        rackCopy.erase(it);
    }
    
    move.tilesUsed = lettersNeeded;
    
    // Kiểm tra các từ cross được tạo ra có hợp lệ không
    return validateAllFormedWords(Board, move.positions, word);
}

int HintSystem::calculateWordScore(Tile* Board[15][15], const string& word, int x, int y, bool horizontal,
                                  const vector<pair<int, int>>& newTilePositions) {
    int wordScore = 0;
    int wordMultiplier = 1;
    
    // Tính điểm từ chính
    for (size_t i = 0; i < word.length(); i++) {
        int posX = horizontal ? x + i : x;
        int posY = horizontal ? y : y + i;
        
        int letterScore = getTileScore(word[i]);
        bool isNewTile = find(newTilePositions.begin(), newTilePositions.end(), 
                             make_pair(posX, posY)) != newTilePositions.end();
        
        if (isNewTile) {
            BonusType bonus = board_bonus[posX][posY];
            switch (bonus) {
                case DOUBLE_LETTER: letterScore *= 2; break;
                case TRIPLE_LETTER: letterScore *= 3; break;
                case DOUBLE_WORD: wordMultiplier *= 2; break;
                case TRIPLE_WORD: wordMultiplier *= 3; break;
                default: break;
            }
        }
        
        wordScore += letterScore;
    }
    
    wordScore *= wordMultiplier;
    
    // Tính điểm các từ cross
    for (const auto& pos : newTilePositions) {
        vector<string> crossWords = findCrossWords(Board, pos.first, pos.second, word[horizontal ? pos.first - x : pos.second - y]);
        for (const string& crossWord : crossWords) {
            if (crossWord.length() > 1) {
                // Tính điểm từ cross (simplified)
                for (char c : crossWord) {
                    wordScore += getTileScore(c);
                }
            }
        }
    }
    
    // Bonus 50 điểm nếu dùng hết 7 tiles
    if (newTilePositions.size() == 7) {
        wordScore += 50;
    }
    
    return wordScore;
}

bool HintSystem::validateAllFormedWords(Tile* Board[15][15], const vector<pair<int, int>>& newTilePositions,
                                       const string& mainWord) {
    // Simplified validation - chỉ check main word
    return dictionary->isWordInDictionary(mainWord);
}

vector<string> HintSystem::findCrossWords(Tile* Board[15][15], int x, int y, char newLetter) {
    vector<string> crossWords;
    
    // Check horizontal cross word
    string hWord = "";
    int startX = x;
    while (startX > 0 && (Board[startX-1][y] != nullptr || startX-1 == x)) startX--;
    
    for (int i = startX; i < 15; i++) {
        if (i == x) {
            hWord += newLetter;
        } else if (Board[i][y] != nullptr) {
            hWord += Board[i][y]->letter;
        } else {
            break;
        }
    }
    
    if (hWord.length() > 1) crossWords.push_back(hWord);
    
    // Check vertical cross word  
    string vWord = "";
    int startY = y;
    while (startY > 0 && (Board[x][startY-1] != nullptr || startY-1 == y)) startY--;
    
    for (int i = startY; i < 15; i++) {
        if (i == y) {
            vWord += newLetter;
        } else if (Board[x][i] != nullptr) {
            vWord += Board[x][i]->letter;
        } else {
            break;
        }
    }
    
    if (vWord.length() > 1) crossWords.push_back(vWord);
    
    return crossWords;
}

bool HintSystem::hasAdjacentTile(Tile* Board[15][15], int x, int y) {
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        
        if (nx >= 0 && nx < 15 && ny >= 0 && ny < 15) {
            if (Board[nx][ny] != nullptr) {
                return true;
            }
        }
    }
    return false;
}

int HintSystem::getTileScore(char letter) {
    TileBag tempBag;  // Temporary để lấy score
    return tempBag.getTileScore(letter);
}