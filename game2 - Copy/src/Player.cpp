#include "Player.hpp"
#include <SDL_image.h>
#include "Game.hpp"
#include "Tile.hpp"
#include<SDL2/SDL_ttf.h>
#include <set>
#include<cmath>
using namespace std;

Player::Player(SDL_Renderer* renderer) : renderer(renderer) {

}

void Player::load_player_rack(const string& path){
   SDL_Surface* surface = IMG_Load(path.c_str());
   rack = SDL_CreateTextureFromSurface(renderer,surface);

   SDL_FreeSurface(surface);

};



void Player::generate_letters(){
   for(int i=0;i<letters_size;i++){
      Tile tile;
      tile.letter = 'A' + i;
      tile.rect.x = 60+i*tile_size;
      tile.rect.y = 500;
      letters.push_back(tile);
   }

   for(int i=0;i<letters_size;i++){
      Tile tile;
      tile.letter = 'G' + i;
      tile.rect.x = 1095+i*tile_size;
      tile.rect.y = 500;
       player2_letters.push_back(tile);
   }
}

void Player::renderText(int x,int y,TTF_Font* font,string text){
    SDL_Color color = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = {x, y, surface->w, surface->h}; // Adjust position as needed
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Player::render(){
    //render frame
    SDL_Surface* frame_surface = IMG_Load("frame.png");
    SDL_Texture* frame_texture = SDL_CreateTextureFromSurface(renderer,frame_surface);
    SDL_Rect frame_rect = {15,210,370,420};
    SDL_RenderCopy(renderer,frame_texture,nullptr,&frame_rect);
    SDL_FreeSurface(frame_surface);
    SDL_DestroyTexture(frame_texture);

    //render text
    TTF_Font* font = TTF_OpenFont("ARIAL.TTF",24);
    renderText(160,250,font,"Player 1");
    renderText(70,400,font,"Score:");
    renderText(70,430,font,"Time:");

    //render player 2 frame
    SDL_Surface* player2_frame_surface = IMG_Load("frame.png");
    SDL_Texture* player2_frame_texture = SDL_CreateTextureFromSurface(renderer,player2_frame_surface);
    SDL_Rect player2_frame_rect = {1050,210,370,420};
    SDL_RenderCopy(renderer,player2_frame_texture,nullptr,&player2_frame_rect);
    SDL_FreeSurface(player2_frame_surface);
    SDL_DestroyTexture(player2_frame_texture);

    //render player 2 text
    renderText(1200,250,font,"Player 2");
    renderText(1105,400,font,"Score:");
    renderText(1105,430,font,"Time:");


    //render player's rack
    for (int i = 0; i < letters_size; i++) {
    int index = letters[i].letter - 'A';
    int player2_index = player2_letters[i].letter - 'A';


    SDL_Rect rack_rect = {letters[i].rect.x, letters[i].rect.y, tile_size, tile_size };
    SDL_Rect dst_rect = { index * tile_size, 0, tile_size, tile_size };

    SDL_Rect player2_rack_rect = {player2_letters[i].rect.x, player2_letters[i].rect.y, tile_size, tile_size };
    SDL_Rect player2_dst_rect = { player2_index * tile_size, 0, tile_size, tile_size };
    // Store the draw position into the tile's rect
    letters[i].rect = rack_rect;
    player2_letters[i].rect = player2_rack_rect;

    SDL_RenderCopy(renderer, rack, &player2_dst_rect, &player2_rack_rect);
    SDL_RenderCopy(renderer, rack, &dst_rect, &rack_rect);
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
    int tileScore = tile -> score;
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
    switch (c) {
        case 'A': case 'E': case 'I': case 'L': case 'N': case 'O': case 'R': case 'S': case 'T': case 'U':
            return 1;
        case 'B': case 'C': case 'D': case 'M': case 'P':
            return 3;
        case 'F': case 'G': case 'H': case 'V': case 'W': case 'Y':
            return 4;
        case 'K':
            return 5;
        case 'J': case 'X':
            return 8;
        case 'Q': case 'Z':
            return 10;
        default:
            return 0;
    }
}


std::pair<bool, int> Player::canSubmitAndCalculateScore(Tile* Board[15][15], vector<pair<int, int>> tile_positions) {
    bool hasAdjacentTile = false;
    Graph* graph = Graph::getInstance();
    int total_score = 0;
    bool allWordsValid = true;
    std::set<std::string> validWords; // Dùng set để tránh trùng từ

    for (auto& pos : tile_positions) {
        int x = pos.first;
        int y = pos.second;
        if ((x > 0 && Board[x - 1][y] != nullptr) ||
            (x < 14 && Board[x + 1][y] != nullptr) ||
            (y > 0 && Board[x][y - 1] != nullptr) ||
            (y < 14 && Board[x][y + 1] != nullptr)) {
            hasAdjacentTile = true;
        }
    }
    if (!hasAdjacentTile) return {false, 0};

    bool vertical = isVertical(tile_positions);

    std::string mainWord;
    int mainScore = 0;
    int doubleWordCount = 0, tripleWordCount = 0;
    std::set<std::pair<int, int>> newTiles(tile_positions.begin(), tile_positions.end());

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

    if (mainWord.empty() || !graph->isWordInDictionary(mainWord)) {
        allWordsValid = false;
    }
    else validWords.insert(mainWord);

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
