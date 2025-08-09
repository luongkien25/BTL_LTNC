// Game.cpp - Fixed version with tile recall and proper rack positioning
#include <ctime>
#include "Game.hpp"
#include "Board.hpp"
#include "Player.hpp"
#include "UI.hpp"
#include <iostream>
#include <tinyxml2.h>
#include <SDL2/SDL_ttf.h>
using namespace std;

bool Game::init(const char* title, int width, int height) {
    TTF_Init();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!window) return false;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_GetRendererOutputSize(renderer, &screenW, &screenH);
    if (!renderer) return false;

    board = new Board(renderer);
    board->load_bonus_from_txt("standard-board.txt");
    board->load("board.jpg", renderer);

    player = new Player(renderer);
    player->load_player_rack("rack.png");

    menu = new Menu(renderer);
    ui = new UI(renderer);

    return true;
}

void Game::run() {
    player->generate_letters();
    current_time = SDL_GetTicks();

    while (isRunning) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}

void Game::cleanup() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::handleEvents() {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (e.type == SDL_QUIT) isRunning = false;
        else if (e.type == SDL_MOUSEBUTTONDOWN) {
            menu->handleEvent(e, mouseX, mouseY);
            player->handleEvent(e, mouseX, mouseY);

            int x = mouseX;
            int y = mouseY;
            SDL_Point mousePoint = {x, y};
            bool check_tile = false;
            bool board_tile_selected = false;

            // ============= FIX: LOGIC MỚI CHO RECALL TILE =============
            // Kiểm tra click vào TỪNG TILE TRỐNG trong rack thay vì toàn bộ rack area
            bool recalledTile = false;

            if (player->has_selected_board_tile) {
                // Player 1 - kiểm tra click vào tile trống trong rack
                if (player->is_first_player_turn) {
                    for (auto& rackTile : player->letters) {
                        if (rackTile.letter == '\0' && SDL_PointInRect(&mousePoint, &rackTile.rect)) {
                            // Recall tile về vị trí trống này
                            int board_x = player->selected_board_tile_x;
                            int board_y = player->selected_board_tile_y;

                            Tile* boardTile = board->boardTile[board_x][board_y];
                            if (boardTile != nullptr) {
                                rackTile.letter = boardTile->letter;
                                rackTile.score = boardTile->score;
                                rackTile.selected = false;

                                delete board->boardTile[board_x][board_y];
                                board->boardTile[board_x][board_y] = nullptr;

                                player->tile_positions.erase(
                                    std::remove_if(player->tile_positions.begin(), player->tile_positions.end(),
                                        [board_x, board_y](const std::pair<int, int>& pos) {
                                            return pos.first == board_x && pos.second == board_y;
                                        }),
                                    player->tile_positions.end()
                                );

                                SDL_Log("Recalled tile '%c' from (%d,%d) to P1 rack",
                                        rackTile.letter, board_x, board_y);
                                player->repositionRackTiles();
                                recalledTile = true;
                            }
                            break;
                        }
                    }
                } else {
                    // Player 2 - kiểm tra click vào tile trống trong rack
                    for (auto& rackTile : player->player2_letters) {
                        if (rackTile.letter == '\0' && SDL_PointInRect(&mousePoint, &rackTile.rect)) {
                            int board_x = player->selected_board_tile_x;
                            int board_y = player->selected_board_tile_y;

                            Tile* boardTile = board->boardTile[board_x][board_y];
                            if (boardTile != nullptr) {
                                rackTile.letter = boardTile->letter;
                                rackTile.score = boardTile->score;
                                rackTile.selected = false;

                                delete board->boardTile[board_x][board_y];
                                board->boardTile[board_x][board_y] = nullptr;

                                player->tile_positions.erase(
                                    std::remove_if(player->tile_positions.begin(), player->tile_positions.end(),
                                        [board_x, board_y](const std::pair<int, int>& pos) {
                                            return pos.first == board_x && pos.second == board_y;
                                        }),
                                    player->tile_positions.end()
                                );

                                SDL_Log("Recalled tile '%c' from (%d,%d) to P2 rack",
                                        rackTile.letter, board_x, board_y);
                                player->repositionRackTiles();
                                recalledTile = true;
                            }
                            break;
                        }
                    }
                }

                // Reset selection nếu đã recall thành công
                if (recalledTile) {
                    player->has_selected_board_tile = false;
                }
            }

            // ============= LOGIC CHỌN TILE TRÊN BOARD =============
            // Xử lý việc chọn tile trên board để di chuyển (chỉ khi không recall)
            if (!recalledTile && SDL_PointInRect(&mousePoint, &board->board_rect)) {
                int board_x = (mousePoint.x - board->board_rect.x) / 40;
                int board_y = (mousePoint.y - board->board_rect.y) / 40;

                if (board_x >= 0 && board_x < 15 && board_y >= 0 && board_y < 15) {
                    Tile* clickedBoardTile = board->boardTile[board_x][board_y];

                    if (clickedBoardTile != nullptr) {
                        bool isTileFromCurrentTurn = false;
                        for (const auto& pos : player->tile_positions) {
                            if (pos.first == board_x && pos.second == board_y) {
                                isTileFromCurrentTurn = true;
                                break;
                            }
                        }

                        if (isTileFromCurrentTurn) {
                            SDL_Log("Selected board tile at (%d, %d): %c", board_x, board_y, clickedBoardTile->letter);

                            // Deselect tất cả tile trong rack
                            for (auto& tile : player->letters) {
                                tile.selected = false;
                            }
                            for (auto& tile : player->player2_letters) {
                                tile.selected = false;
                            }

                            player->selected_board_tile_x = board_x;
                            player->selected_board_tile_y = board_y;
                            player->has_selected_board_tile = true;

                            board_tile_selected = true;
                            SDL_Log("Board tile selected for movement");
                        }
                    }
                }
            }

            // ============= LOGIC CHỌN TILE TRONG RACK =============
            // Handle player tile selection - CHỈ XỬ LÝ KHI KHÔNG CÓ BOARD TILE SELECTED VÀ KHÔNG RECALL
            if (!board_tile_selected && !recalledTile && player->is_first_player_turn) {
                for (auto& tile : player->letters) {
                    if (tile.letter != '\0' && SDL_PointInRect(&mousePoint, &tile.rect)) {
                        for (auto& otherTile : player->letters) {
                            otherTile.selected = false;
                        }
                        tile.selected = true;
                        check_tile = true;
                        player->has_selected_board_tile = false;
                        SDL_Log("Selected P1 tile: %c", tile.letter);
                        break;
                    }
                }
            }

            bool player2_check_tile = false;
            if (!board_tile_selected && !recalledTile && !player->is_first_player_turn) {
                for (auto& tile : player->player2_letters) {
                    if (tile.letter != '\0' && SDL_PointInRect(&mousePoint, &tile.rect)) {
                        for (auto& otherTile : player->player2_letters) {
                            otherTile.selected = false;
                        }
                        tile.selected = true;
                        player2_check_tile = true;
                        player->has_selected_board_tile = false;
                        SDL_Log("Selected P2 tile: %c", tile.letter);
                        break;
                    }
                }
            }

            // ============= LOGIC ĐẶT/DI CHUYỂN TILE LÊN BOARD =============
            // Handle placing/moving tiles on board (chỉ khi không recall)
            if (!check_tile && !player2_check_tile && !board_tile_selected && !recalledTile &&
                SDL_PointInRect(&mousePoint, &board->board_rect)) {

                mousePoint.x = ((mousePoint.x - board->board_rect.x) / 40) * 40 + (board->board_rect.x);
                mousePoint.y = ((mousePoint.y - board->board_rect.y) / 40) * 40 + (board->board_rect.y);

                int new_x = (mousePoint.x - board->board_rect.x) / 40;
                int new_y = (mousePoint.y - board->board_rect.y) / 40;

                // CASE 1: Di chuyển tile đã có trên board
                if (player->has_selected_board_tile) {
                    int old_x = player->selected_board_tile_x;
                    int old_y = player->selected_board_tile_y;

                    SDL_Log("Attempting to move tile from (%d,%d) to (%d,%d)", old_x, old_y, new_x, new_y);

                    if (new_x >= 0 && new_x < 15 && new_y >= 0 && new_y < 15 &&
                        board->boardTile[new_x][new_y] == nullptr &&
                        (old_x != new_x || old_y != new_y)) {

                        Tile* movingTile = board->boardTile[old_x][old_y];
                        if (movingTile != nullptr) {
                            movingTile->rect.x = mousePoint.x;
                            movingTile->rect.y = mousePoint.y;

                            board->boardTile[new_x][new_y] = movingTile;
                            board->boardTile[old_x][old_y] = nullptr;

                            for (auto& pos : player->tile_positions) {
                                if (pos.first == old_x && pos.second == old_y) {
                                    pos.first = new_x;
                                    pos.second = new_y;
                                    break;
                                }
                            }

                            SDL_Log("Successfully moved tile '%c' from (%d,%d) to (%d,%d)",
                                    movingTile->letter, old_x, old_y, new_x, new_y);
                        }
                    } else {
                        SDL_Log("Cannot move tile: position occupied or invalid");
                    }

                    player->has_selected_board_tile = false;
                }
                // CASE 2: Đặt tile mới từ rack
                else {
                    // Handle placing tiles from player 1 rack
                    for (auto& tile : player->letters) {
                        if (tile.selected && tile.letter != '\0') {
                            if (new_x >= 0 && new_x < 15 && new_y >= 0 && new_y < 15 &&
                                board->boardTile[new_x][new_y] == nullptr) {

                                tile.rect.x = mousePoint.x;
                                tile.rect.y = mousePoint.y;

                                board->update_board_with_tile(&tile, new_x, new_y);
                                player->tile_positions.push_back({new_x, new_y});
                                tile.selected = false;

                                SDL_Log("Placed P1 tile '%c' at (%d,%d)", tile.letter, new_x, new_y);

                                // ĐỂ TRỐNG VỊ TRÍ TRONG RACK (để có thể recall về)
                                tile.letter = '\0';
                            }
                            break;
                        }
                    }

                    // Handle placing tiles from player 2 rack
                    for (auto& tile : player->player2_letters) {
                        if (tile.selected && tile.letter != '\0') {
                            if (new_x >= 0 && new_x < 15 && new_y >= 0 && new_y < 15 &&
                                board->boardTile[new_x][new_y] == nullptr) {

                                tile.rect.x = mousePoint.x;
                                tile.rect.y = mousePoint.y;

                                board->update_board_with_tile(&tile, new_x, new_y);
                                player->tile_positions.push_back({new_x, new_y});
                                tile.selected = false;

                                SDL_Log("Placed P2 tile '%c' at (%d,%d)", tile.letter, new_x, new_y);

                                // ĐỂ TRỐNG VỊ TRÍ TRONG RACK (để có thể recall về)
                                tile.letter = '\0';
                            }
                            break;
                        }
                    }
                }
            }

            // ============= SUBMIT BUTTON =============
            SDL_Rect submitRect = {540, 750, 76, 43};
            if (SDL_PointInRect(&mousePoint, &submitRect)) {
                SDL_Log("=== SUBMIT DEBUG ===");
                SDL_Log("Tile positions count: %zu", player->tile_positions.size());

                if (!player->tile_positions.empty()) {
                    auto [allWordValids, score] = player->canSubmitAndCalculateScore(board->boardTile, player->tile_positions);

                    SDL_Log("Validation result: %s", allWordValids ? "VALID" : "INVALID");
                    SDL_Log("Calculated score: %d", score);

                    if (allWordValids && score > 0) {
                        SDL_Log("=== SUBMIT SUCCESSFUL ===");

                        if (player->is_first_player_turn) {
                            player->player1_score += score;
                            SDL_Log("Player 1 score updated: %d (+%d)", player->player1_score, score);
                        } else {
                            player->player2_score += score;
                            SDL_Log("Player 2 score updated: %d (+%d)", player->player2_score, score);
                        }

                        // Refill rack
                        if (player->is_first_player_turn) {
                            player->refillRack(1);
                            SDL_Log("Player 1 rack refilled");
                        } else {
                            player->refillRack(2);
                            SDL_Log("Player 2 rack refilled");
                        }

                        player->is_first_player_turn = !player->is_first_player_turn;
                        SDL_Log("Turn switched to: %s", player->is_first_player_turn ? "Player 1" : "Player 2");

                        player->tile_positions.clear();
                        SDL_Log("Tile positions cleared");

                        current_time = SDL_GetTicks();

                        SDL_Log("=== SUBMIT COMPLETED ===");

                    } else {
                        SDL_Log("=== SUBMIT FAILED ===");
                        if (!allWordValids) {
                            SDL_Log("Reason: Invalid words formed");
                        }
                        if (score <= 0) {
                            SDL_Log("Reason: No score calculated");
                        }
                    }
                } else {
                    SDL_Log("No tiles placed. Cannot submit.");
                }
            }

            // ============= EXCHANGE BUTTON =============
            SDL_Rect exchangeRect = {625, 750, 76, 43};
            if (SDL_PointInRect(&mousePoint, &exchangeRect)) {
                if (player->is_first_player_turn) {
                    std::vector<int> tilesToExchange;
                    for (size_t i = 0; i < player->letters.size(); i++) {
                        if (player->letters[i].selected) {
                            tilesToExchange.push_back(i);
                        }
                    }
                    if (!tilesToExchange.empty()) {
                        player->exchangeTiles(1, tilesToExchange);
                        player->is_first_player_turn = false;
                        SDL_Log("Player 1 exchanged %zu tiles", tilesToExchange.size());
                    }
                } else {
                    std::vector<int> tilesToExchange;
                    for (size_t i = 0; i < player->player2_letters.size(); i++) {
                        if (player->player2_letters[i].selected) {
                            tilesToExchange.push_back(i);
                        }
                    }
                    if (!tilesToExchange.empty()) {
                        player->exchangeTiles(2, tilesToExchange);
                        player->is_first_player_turn = true;
                        SDL_Log("Player 2 exchanged %zu tiles", tilesToExchange.size());
                    }
                }
            }
            SDL_Rect resign_button_rect = {795, 750, 76, 43};
            if (SDL_PointInRect(&mousePoint, &resign_button_rect)){
                    is_showing_win_effect = true;
                       is_clicked_submit = true;
            }
        }
    }
}

void Game::update() {
    player->handle_timer(current_time);

    if (player->getRemainingTilesCount() == 0) {
        SDL_Log("Tile bag is empty!");
        is_showing_win_effect = true;
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    for (auto& opt : menu->Option) {
        if (opt.selected) {
            if (opt.option == "Multiple Players") {
               if(is_showing_win_effect) game_state = 3;
                else game_state = 1;
            }
        }
    }

    switch (game_state) {
        case 0:
            menu->render();
            break;

        case 1:{
            board->render(screenH, screenW, player->getRackTexture());
            player->render();

            ui->render_buttons("submit_button.png", {540, 750, 76, 43});
            ui->render_buttons("exchange_word.png", {625, 750, 76, 43});
            ui->render_buttons("hint_button.png", {710, 750, 76, 43});
            ui->render_buttons("resign.png", {795, 750, 76, 43});
            ui->render_buttons("home.png", {1200, 20, 64, 64});

            TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 20);
            if (font) {
                string turnText = player->is_first_player_turn ? "Player 1's Turn" : "Player 2's Turn";
                SDL_Color white = {255, 255, 255};
                SDL_Surface* surface = TTF_RenderText_Blended(font, turnText.c_str(), white);
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_Rect textRect = {screenW/2 - 100, 50, surface->w, surface->h};
                    SDL_RenderCopy(renderer, texture, nullptr, &textRect);
                    SDL_FreeSurface(surface);
                    SDL_DestroyTexture(texture);
                }
                TTF_CloseFont(font);
            }
            break;
        }
        case 3:
            TTF_Font* font = TTF_OpenFont("arial.ttf", 48);
            string winner;
            if(is_clicked_submit){
                if(!player->is_first_player_turn){
                winner = "Player 1 Win!";
                }
                else
                    winner = "Player 2 Win!";

             }

            else winner = player->player1_score>player->player2_score ?  "Player 1 Win!" :  "Player 2 Win!";
            showWinningEffect(renderer, font, 800, 600,winner);
            is_showing_menu = true;
            is_showing_win_effect = false;
            game_state = 0;
            for (auto& opt : menu->Option) {
                if (opt.selected) {
                    if (opt.option == "Multiple Players") {
                       opt.selected = false;
                    }
                }
            }
            break;

    }

    SDL_RenderPresent(renderer);
}
