#pragma once

#include "vec.h"
#include "tilemap.h"

#include <string>
#include <map>

using TileLocations = std::map<Vec<int>, std::string>;
using EnemyLocations = std::map<Vec<float>, std::string>;

class Level {
public:
    Level(std::string name);

    // data
    std::string name;
    int width{0};
    int height{0};
    std::vector<std::string> tile_filenames;
    TileLocations tile_locations;
    std::map<std::string, Tile> tile_types;

    Vec<int> player_spawn_location{-1, -1};
    EnemyLocations enemy_locations;
};