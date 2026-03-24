#pragma once
#include "vec.h"
#include "sprite.h"
#include "json.hpp"
#include "graphics.h"
#include "physics.h"


// need to map Vec to JSON
template <typename T>
void to_json(const nlohmann::json& j, const Vec<T>& v) {
    j = nlohmann::json::array({v.x, v.y});
}

template <typename T>
void from_json(const nlohmann::json& j, Vec<T>& v) {
    v.x = j.at(0).get<T>();
    v.y = j.at(1).get<T>();
}

// needed for JSON conversions
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Sprite, name, filename, location, size, scale, dt_per_frame, number_of_frames);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Physics, position, velocity, acceleration, gravity, damping, walk_acceleration, jump_velocity, terminal_velocity);

class GameObject;
namespace AssetManager {
    void get_game_object_details(const std::string& name, Graphics& graphics, GameObject& obj);
}