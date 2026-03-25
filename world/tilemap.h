#pragma once
#include <vector>

#include "sprite.h"

class Tile {
public:
    Sprite sprite;
    bool blocking{false};
    std::string id;
    std::string event_name;
};

class Tilemap {
public:
    Tilemap(int width, int height);

    // Access tiles
    const Tile& operator()(int x, int y) const;
    Tile& operator()(int x, int y);

    const int width;
    const int height;

private:
    std::vector<Tile> tiles;

    void check_bounds(int x, int y) const;
};