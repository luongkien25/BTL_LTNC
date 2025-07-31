
#include "menu.hpp"

using namespace std;
Menu::Menu(SDL_Renderer* renderer) : renderer(renderer) {
    // Optional: initialization logic here
}
Option opt;
void Menu::renderText(int x,int y,TTF_Font* font,string text){
    SDL_Color color = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = {x, y, surface->w, surface->h}; // Adjust position as needed
    opt.dstRect = dstRect;
    opt.option = text;
    Option.push_back(opt);
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


void Menu::handleEvent(SDL_Event& event,int& mouseX,int& mouseY){


    SDL_Point mousePoint = {mouseX,mouseY};
    while(SDL_PollEvent(&event)){
         for(auto& opt: Option){
                if(SDL_PointInRect(&mousePoint,&opt.dstRect)){
                    opt.selected = true;
                }

            }

    }
}

void Menu::render(){

    int screenW, screenH;
    SDL_GetRendererOutputSize(renderer, &screenW, &screenH);
    SDL_Surface* back_ground_surface = IMG_Load("background_menu.jpg");
    if(!back_ground_surface) cerr<<IMG_GetError();
    SDL_Texture* back_ground_texture = SDL_CreateTextureFromSurface(renderer,back_ground_surface);
    SDL_Rect background_dst = {0,0,screenW,screenH};
    SDL_FreeSurface(back_ground_surface);
    SDL_RenderCopy(renderer,back_ground_texture,nullptr,&background_dst);

    TTF_Font* font = TTF_OpenFont("ARIAL.TTF",40);


    renderText(screenW/2-200,screenH/2+20,font,"Setting");
    renderText(screenW/2-200,screenH/2-60,font,"Player with computer");
    renderText(screenW/2-200,screenH/2-140,font,"Multiple Players");
}

