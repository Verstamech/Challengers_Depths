#pragma once

#include "graphics.h"
#include "vec.h"
#include "sprite.h"
#include "json.hpp"
#include "physics.h"
#include "level.h"
#include "world.h"

class GameObject;

// need to map Vec to JSON
template <typename T>
void to_json(nlohmann::json& j, const Vec<T>& v) {
    j = nlohmann::json::array({v.x, v.y});
}

template <typename T>
void from_json(const nlohmann::json& j, Vec<T>& v) {
    v.x = j.at(0).get<T>();
    v.y = j.at(1).get<T>();
}

// json for Level
inline void to_json(nlohmann::json& j, const Level& level) {
    j["width"] = level.width;
    j["height"] = level.height;
    j["tile_filenames"] = level.tile_filenames;
    j["player_spawn_location"] = level.player_spawn_location;
    j["tiles"] = nlohmann::json::array();
    for (const auto& [pos, tile] : level.tile_locations) {
        j["tiles"].push_back({
            {"pos", pos},
            {"tile", tile}
        });
    }

    for (const auto& [pos, enemy] : level.enemy_locations) {
        j["enemies"].push_back({
        {"pos", pos},
        {"enemy", enemy}
        });
    }
}
inline void from_json(const nlohmann::json& j, Level& level) {
    level.width = j.at("width").get<int>();
    level.height = j.at("height").get<int>();
    level.tile_filenames = j.at("tile_filenames").get<std::vector<std::string>>();
    level.player_spawn_location = j.contains("player_spawn_location") ? j.at("player_spawn_location").get<Vec<int>>() : Vec<int>{-1, -1};
    if (j.contains("tiles")) {
        for (const auto& t : j.at("tiles")) {
            Vec<int> pos = t.at("pos").get<Vec<int>>();
            std::string tile_id = t.at("tile").get<std::string>();
            level.tile_locations[pos] = tile_id;
        }
    }

    if (j.contains("enemies")) {
        for (const auto& e : j.at("enemies")) {
            Vec<float> pos = e.at("pos").get<Vec<float>>();
            std::string enemy_name = e.at("enemy").get<std::string>();
            level.enemy_locations[pos] = enemy_name;
        }
    }
}

// these are for the json library - NOTE: if I want this to be more flexible, I create my own to/from json functions and can provide default values. Then json is strict formatted
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Sprite, name, filename, location, size, scale, dt_per_frame, number_of_frames);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Physics, velocity, acceleration, gravity, damping, walk_acceleration, jump_velocity, terminal_velocity);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Tile, sprite, event_name, blocking);

namespace AssetManager {
    void get_game_object_details(const std::string& name, Graphics& graphics, GameObject& obj, bool random_start = false);
    void get_level_details(Graphics& graphics, Level& level);
    void update_level_details(const Level& level);
    void get_available_items(const std::string& filename, Graphics& graphics, World& world);
}