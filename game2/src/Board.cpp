#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <Board.hpp>
#include "tinyxml2.h"
#include <SDL_image.h>
#include <string>
#include <filesystem>
#include <sstream>
#include <Game.hpp>
using namespace std;
using namespace tinyxml2;
BonusType board_bonus[15][15];
Board::Board(SDL_Renderer* renderer) : renderer(renderer) {
    // Optional: initialization logic here
}

void Board::load_bonus_from_txt(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        SDL_Log("Failed to load board layout: %s", path.c_str());
        return;
    }

    std::string line;
    for (int row = 0; row < 15 && std::getline(file, line); row++) {
        for (int col = 0; col < 15 && col < (int)line.size(); col++) {
            switch (line[col]) {
                case 't': board_bonus[row][col] = TRIPLE_WORD; break;
                case 'd': board_bonus[row][col] = DOUBLE_WORD; break;
                case '2': board_bonus[row][col] = DOUBLE_LETTER; break;
                case '3': board_bonus[row][col] = TRIPLE_LETTER; break;
                default:  board_bonus[row][col] = NONE; break;
            }
        }
    }
}

void Board::load(const string& file, SDL_Renderer* renderer) {
    SDL_Surface* tempSurface = IMG_Load(file.c_str());
    if (!tempSurface) {
        SDL_Log("Failed to load image %s: %s", file.c_str(), IMG_GetError());
        return;
    }

    board_texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    if (!board_texture) {
        SDL_Log("Failed to create texture from %s: %s", file.c_str(), SDL_GetError());
    }


}


// Trong Board.cpp - cập nhật method render để highlight tile được chọn
// Trong Board.cpp - chỉ cần sửa method render hiện có
void Board::render(int& screenW, int& screenH, SDL_Texture* rackTexture) {
    board_rect = {screenW/2-33,screenH/2-board_size_y,board_size_x,board_size_y};
    SDL_RenderCopy(renderer, board_texture, nullptr, &board_rect);
    
    // Render tiles that have been placed on the board
    for (int x = 0; x < 15; x++) {
        for (int y = 0; y < 15; y++) {
            if (boardTile[x][y] != nullptr && boardTile[x][y]->letter != '\0') {
                Tile* tile = boardTile[x][y];
                
                // Tính toán vị trí chính xác trên board
                int tileX = board_rect.x + x * 40;
                int tileY = board_rect.y + y * 40;
                
                SDL_Rect tileRect = {tileX, tileY, 40, 40};
                
                // SỬ DỤNG RACK TEXTURE TỪ PLAYER CLASS
                if (rackTexture) {
                    // Render tile với texture avatar giống như trong rack
                    int index = tile->letter - 'A';
                    if (index < 0 || index >= 26) index = 0; // Handle special characters
                    
                    SDL_Rect srcRect = {index * 40, 0, 40, 40}; // 40 là tile_size
                    SDL_RenderCopy(renderer, rackTexture, &srcRect, &tileRect);
                } else {
                    // Fallback: render như cũ nếu không có rackTexture
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderFillRect(renderer, &tileRect);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderDrawRect(renderer, &tileRect);
                    
                    TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 20);
                    if (font) {
                        SDL_Color black = {0, 0, 0};
                        string letterStr(1, tile->letter);
                        SDL_Surface* textSurface = TTF_RenderText_Blended(font, letterStr.c_str(), black);
                        if (textSurface) {
                            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                            SDL_Rect textRect = {
                                tileX + 15, 
                                tileY + 10, 
                                textSurface->w, 
                                textSurface->h
                            };
                            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                            SDL_FreeSurface(textSurface);
                            SDL_DestroyTexture(textTexture);
                        }
                        TTF_CloseFont(font);
                    }
                }
            }
        }
    }
}

// Thêm method mới để render với selection highlight
void Board::renderWithSelection(int& screenW, int& screenH, SDL_Texture* rackTexture, 
                               bool hasSelection, int selectedX, int selectedY) {
    // Render board như bình thường trước
    render(screenW, screenH, rackTexture);
    
    // Sau đó thêm highlight cho tile được chọn
    if (hasSelection && selectedX >= 0 && selectedX < 15 && selectedY >= 0 && selectedY < 15) {
        if (boardTile[selectedX][selectedY] != nullptr) {
            int tileX = board_rect.x + selectedX * 40;
            int tileY = board_rect.y + selectedY * 40;
            SDL_Rect tileRect = {tileX, tileY, 40, 40};
            
            // Vẽ overlay màu vàng với alpha
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 100); 
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_RenderFillRect(renderer, &tileRect);
            
            // Vẽ viền đỏ để làm nổi bật
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            for (int i = 0; i < 3; i++) { // Vẽ viền dày hơn
                SDL_Rect borderRect = {tileX - i, tileY - i, 40 + 2*i, 40 + 2*i};
                SDL_RenderDrawRect(renderer, &borderRect);
            }
        }
    }
}


void Board::update_board_with_tile(Tile* tile, int x, int y) {
    if (x >= 0 && x < 15 && y >= 0 && y < 15 && boardTile[x][y] == nullptr) {
        // TẠO BẢN COPY CỦA TILE THAY VÌ SỬ DỤNG POINTER TRỰC TIẾP
        Tile* newTile = new Tile();
        newTile->letter = tile->letter;
        newTile->score = tile->score;
        newTile->rect.x = tile->rect.x;
        newTile->rect.y = tile->rect.y;
        newTile->rect.w = tile->rect.w;
        newTile->rect.h = tile->rect.h;
        newTile->selected = false;
        
        boardTile[x][y] = newTile;
        SDL_Log("Tile '%c' (score: %d) placed at position (%d, %d)", 
                newTile->letter, newTile->score, x, y);
    } else {
        if (x < 0 || x >= 15 || y < 0 || y >= 15) {
            SDL_Log("Invalid board position: (%d, %d)", x, y);
        } else {
            SDL_Log("Tile already exists at position (%d, %d)", x, y);
        }
    }
}

Board::~Board() {
    // Giải phóng memory của các tile trên board
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            if (boardTile[i][j] != nullptr) {
                delete boardTile[i][j];
                boardTile[i][j] = nullptr;
            }
        }
    }
    
    // Giải phóng textures
    if (tileset) {
        SDL_DestroyTexture(tileset);
    }
    if (board_texture) {
        SDL_DestroyTexture(board_texture);
    }
}
