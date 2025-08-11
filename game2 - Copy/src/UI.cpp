#include<UI.hpp>
#include <cstdlib>
#include <algorithm>
UI::UI(SDL_Renderer* renderer):renderer(renderer){

}
void UI::render_buttons(string path,SDL_Rect dstRect){
  SDL_Surface* surface = IMG_Load(path.c_str());
   SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
    // ;
   SDL_RenderCopy(renderer,texture,nullptr,&dstRect);
}


void spawnConfetti(std::vector<Particle> &particles, int screenW, int screenH) {
    for (int i = 0; i < 15; i++) { // more particles for density
        Particle p;
        p.x = rand() % screenW;      // anywhere across full width
        p.y = -5;                    // start above the top
        p.vx = ((rand() % 200) - 100) / 100.0f; // drift left/right
        p.vy = (rand() % 50 + 50) / 10.0f;      // fall speed
        p.color = { (Uint8)(rand()%256), (Uint8)(rand()%256), (Uint8)(rand()%256), 255 };
        p.lifetime = screenH * 20;   // live long enough to reach bottom
        particles.push_back(p);
    }
}

void spawnSparkles(std::vector<Particle> &particles, int screenW, int screenH) {
    for (int i = 0; i < 5; i++) {
        Particle p;
        p.x = rand() % screenW;      // anywhere across full width
        p.y = rand() % screenH;      // anywhere across full height
        p.vx = 0;
        p.vy = 0;
        p.color = { 255, 255, (Uint8)(rand()%128 + 128), 255 };
        p.lifetime = 200; // short flash
        particles.push_back(p);
    }
}


void updateParticles(std::vector<Particle> &particles, int delta) {
    for (auto &p : particles) {
        p.x += p.vx;
        p.y += p.vy;
        p.lifetime -= delta;
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(),
                                   [](const Particle &p) { return p.lifetime <= 0; }),
                    particles.end());
}

void renderParticles(SDL_Renderer *renderer, const std::vector<Particle> &particles) {
    for (const auto &p : particles) {
        SDL_SetRenderDrawColor(renderer, p.color.r, p.color.g, p.color.b, p.color.a);
        SDL_Rect rect = {(int)p.x, (int)p.y, 4, 4};
        SDL_RenderFillRect(renderer, &rect);
    }
}

void showWinningEffect(SDL_Renderer *renderer, TTF_Font *font, int screenW, int screenH,string& winner) {
    SDL_Color gold = {255, 215, 0, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font,winner.c_str(), gold);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    int textW, textH;
    SDL_QueryTexture(texture, NULL, NULL, &textW, &textH);
    SDL_Rect dstRect = { screenW / 2 - textW / 2+300, screenH / 2 - textH / 2, textW, textH };

    std::vector<Particle> particles;
    Uint32 start = SDL_GetTicks();
    Uint32 lastFrame = start;

    while (SDL_GetTicks() - start < 3000) { // 3 seconds
        Uint32 now = SDL_GetTicks();
        int delta = now - lastFrame;
        lastFrame = now;

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return;
        }

        spawnConfetti(particles, 2000, 2000);
        spawnSparkles(particles, 2000, 2000);

        updateParticles(particles, delta);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, texture, NULL, &dstRect);
        renderParticles(renderer, particles);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}
