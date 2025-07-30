#include "Player.hpp"
#include "Tile.hpp"
#include <SDL_image.h>
#include "Game.hpp"
#include<SDL2/SDL_ttf.h>
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
      tile.letter = 'A' + rand()%27;
      tile.rect.x = 50+i*tile_size;
      tile.rect.y = 500;
      letters.push_back(tile);
   }

   for(int i=0;i<letters_size;i++){
      Tile tile;
      tile.letter = 'A' + rand()%27;
      tile.rect.x = 1050+i*tile_size;
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
    SDL_Rect frame_rect = {15,210,410,500};
    SDL_RenderCopy(renderer,frame_texture,nullptr,&frame_rect);
    SDL_FreeSurface(frame_surface);
    SDL_DestroyTexture(frame_texture);

    //render text
    TTF_Font* font = TTF_OpenFont("ARIAL.TTF",24);
    renderText(170,250,font,"Player 1");
    renderText(70,400,font,"Score:");
    renderText(70,430,font,"Time:");

    //render player 2 frame
    SDL_Surface* player2_frame_surface = IMG_Load("player2_frame.png");
    SDL_Texture* player2_frame_texture = SDL_CreateTextureFromSurface(renderer,player2_frame_surface);
    SDL_Rect player2_frame_rect = {1010,210,410,500};
    SDL_RenderCopy(renderer,player2_frame_texture,nullptr,&player2_frame_rect);
    SDL_FreeSurface(player2_frame_surface);
    SDL_DestroyTexture(player2_frame_texture);

    //render player 2 text
    renderText(1170,250,font,"Player 2");
    renderText(1070,400,font,"Score:");
    renderText(1070,430,font,"Time:");


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
    vector<string> validWords;  // Lưu trữ các từ hợp lệ

    // wordMultiplier chỉ cần khai báo một lần
    int wordMultiplier = 1;  // Nhân với bonus từ Double Word và Triple Word

    // Kiểm tra các ô liền kề với các ô đã có trên bảng
    for (auto& pos : tile_positions) {
        int x = pos.first;
        int y = pos.second;

        if (x > 0 && Board[x - 1][y] != nullptr) hasAdjacentTile = true;  
        if (x < 14 && Board[x + 1][y] != nullptr) hasAdjacentTile = true;  
        if (y > 0 && Board[x][y - 1] != nullptr) hasAdjacentTile = true; 
        if (y < 14 && Board[x][y + 1] != nullptr) hasAdjacentTile = true;  
    }

    if (!hasAdjacentTile) {
        return std::make_pair(false, 0);  // Nếu không có tile liền kề, không thể đặt từ
    }

    string word;
    // Kiểm tra từ theo chiều dọc
    if (isVertical(tile_positions)) {
        int x = tile_positions[0].first;
        int y = tile_positions[0].second;
        word = Board[x][y]->letter;
        int score = calculateTileScore(x, y, Board[x][y]);
        int up = x - 1;
        int down = x + 1;

        while (up >= 0 && Board[up][y] != nullptr) {
            word = Board[up][y]->letter + word;
            score += calculateTileScore(up, y, Board[up][y]);
            up--;
        }

        while (down < 15 && Board[down][y] != nullptr) {
            word += Board[down][y]->letter;
            score += calculateTileScore(down, y, Board[down][y]);
            down++;
        }

        if (!word.empty() && !graph->isWordInDictionary(word)) {
            allWordsValid = false;
        } else {
            validWords.push_back(word);  // Lưu từ hợp lệ
            total_score += score;
        }

        // Kiểm tra từ ngang
        for (auto& pos : tile_positions) {
            int x = pos.first;
            int y = pos.second;
            word = Board[x][y]->letter;
            score = calculateTileScore(x, y, Board[x][y]);
            int left = y - 1;
            int right = y + 1;

            while (left >= 0 && Board[x][left] != nullptr) {
                word = Board[x][left]->letter + word;
                score += calculateTileScore(x, left, Board[x][left]);
                left--;
            }

            while (right < 15 && Board[x][right] != nullptr) {
                word += Board[x][right]->letter;
                score += calculateTileScore(x, right, Board[x][right]);
                right++;
            }

            if (!word.empty() && !graph->isWordInDictionary(word)) {
                allWordsValid = false;
                break;
            } else {
                validWords.push_back(word);  // Lưu từ hợp lệ
                total_score += score;
            }
        }
    } else {  // Kiểm tra từ theo chiều ngang
        int x = tile_positions[0].first;
        int y = tile_positions[0].second;
        word = "";
        int score = calculateTileScore(x, y, Board[x][y]);
        int left = y - 1;
        int right = y;

        while (left >= 0 && Board[x][left] != nullptr) {
            word = Board[x][left]->letter + word;
            score += calculateTileScore(x, left, Board[x][left]);
            left--;
        }

        while (right < 15 && Board[x][right] != nullptr) {
            word += Board[x][right]->letter;
            score += calculateTileScore(x, right, Board[x][right]);
            right++;
        }

        if (!word.empty() && !graph->isWordInDictionary(word)) {
            allWordsValid = false;
        } else {
            validWords.push_back(word);  // Lưu từ hợp lệ
            total_score += score;
        }

        // Kiểm tra từ dọc
        for (auto& pos : tile_positions) {
            int x = pos.first;
            int y = pos.second;
            word = Board[x][y]->letter;
            score = calculateTileScore(x, y, Board[x][y]);
            int up = x - 1;
            int down = x + 1;

            while (up >= 0 && Board[up][y] != nullptr) {
                word = Board[up][y]->letter + word;
                score += calculateTileScore(up, y, Board[up][y]);
                up--;
            }

            while (down < 15 && Board[down][y] != nullptr) {
                word += Board[down][y]->letter;
                score += calculateTileScore(down, y, Board[down][y]);
                down++;
            }

            if (!word.empty() && !graph->isWordInDictionary(word)) {
                allWordsValid = false;
                break;
            } else {
                validWords.push_back(word);  // Lưu từ hợp lệ
                total_score += score;
            }
        }
    }

    // Tính điểm cho toàn bộ từ: Áp dụng bonus Double Word, Triple Word
    for (auto& word : validWords) {
        int wordScore = 0;
        for (auto& c : word) {
            wordScore += getTileScore(c);  // Tính điểm cho từ
        }

        // Kiểm tra các ô có Double Word hoặc Triple Word
        int doubleWordCount = 0;
        int tripleWordCount = 0;

        // Kiểm tra các ô có Double Word hoặc Triple Word
        for (auto& pos : tile_positions) {
            int x = pos.first;
            int y = pos.second;
            BonusType bonus = board_bonus[x][y];

            switch (bonus) {
                case DOUBLE_WORD:
                    doubleWordCount++;
                    break;
                case TRIPLE_WORD:
                    tripleWordCount++;
                    break;
                default:
                    break;
            }
        }

        // Áp dụng bonus từ Double Word, Triple Word
        wordScore *= (1 << doubleWordCount) * (3 * tripleWordCount);
        total_score += wordScore;
    }

    // Nếu là Bingo (7 tile), cộng thêm 50 điểm
    if (tile_positions.size() == 7) {
        total_score += 50;
    }

    return std::make_pair(allWordsValid, total_score);
}


