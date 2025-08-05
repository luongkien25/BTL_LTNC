#include<UI.hpp>
UI::UI(SDL_Renderer* renderer):renderer(renderer){

}
void UI::render_buttons(string path,SDL_Rect dstRect){
  SDL_Surface* surface = IMG_Load(path.c_str());
   SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
    // ;
   SDL_RenderCopy(renderer,texture,nullptr,&dstRect);
}
