#include <iostream>
#include <SDL2/SDL.h>
#include <Board.hpp>
#include "tinyxml2.h"
#include <SDL_image.h>
#include <string>
#include <sstream>
using namespace std;
using namespace tinyxml2;

Board::Board(SDL_Renderer* renderer) : renderer(renderer) {
    // Optional: initialization logic here
}

bool Board::load(const string& tmxFile, const string& tilesetImage) {
    XMLDocument doc;
    if (doc.LoadFile(tmxFile.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load TMX file\n";
        return false;
    }

    XMLElement* mapElement = doc.FirstChildElement("map");
    mapWidth = mapElement->IntAttribute("width");
    mapHeight = mapElement->IntAttribute("height");
    tileWidth = mapElement->IntAttribute("tilewidth");
    tileHeight = mapElement->IntAttribute("tileheight");

    XMLElement* layer = mapElement->FirstChildElement("layer");
    XMLElement* data = layer->FirstChildElement("data");
    const char* csv = data->GetText();

    stringstream ss(csv);
    string token;
    while (getline(ss, token, ',')) {
        tileIDs.push_back(stoi(token));
    }

    // Load tileset texture
    SDL_Surface* surface = IMG_Load(tilesetImage.c_str());
    if (!surface) return false;
    tileset = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return true;
}

void Board::render() {
    if (!tileset) return;
    int tilesPerRow = 256 / tileWidth; // Assuming 256px tileset width

    for (int row = 0; row < mapHeight; ++row) {
        for (int col = 0; col < mapWidth; ++col) {
            int tileID = tileIDs[row * mapWidth + col];
            if (tileID == 0) continue;

            SDL_Rect src = {
                ((tileID - 1) % tilesPerRow) * tileWidth,
                ((tileID - 1) / tilesPerRow) * tileHeight,
                tileWidth, tileHeight
            };
            SDL_Rect dest = {
                col * tileWidth,
                row * tileHeight,
                tileWidth, tileHeight
            };
            SDL_RenderCopy(renderer, tileset, &src, &dest);
        }
    }
}
