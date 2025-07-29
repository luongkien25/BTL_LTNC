#include "Player.hpp"
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
