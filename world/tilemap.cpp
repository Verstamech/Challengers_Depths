#include "tilemap.h"
#include <stdexcept>
#include <sstream>

Tilemap::Tilemap(int width, int height)
    : width{width}, height{height}, tiles(width * height) {
    if (width < 1) {
        throw std::runtime_error("Width must be positive");
    }
    if (height < 1) {
        throw std::runtime_error("Height must be positive");
    }
}

void Tilemap::check_bounds(int x, int y) const {
    if (x >= width || x < 0 || y >= height || y < 0) {
        std::stringstream ss;
        ss << "(" << x << ", " << y << ") not within bounds (";
        ss << width << ", " << height << ")";
        throw std::runtime_error(ss.str());
    }
}

const Tile &Tilemap::operator()(int x, int y) const {
    check_bounds(x, y);
    return tiles.at(x + y*width);
}

Tile& Tilemap::operator()(int x, int y) {
    check_bounds(x, y);
    return tiles.at(x + y*width);
}