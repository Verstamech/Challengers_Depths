#include "asset_manager.h"
#include "game_object.h"
#include <filesystem>
#include <fstream>

void convert_sprites(std::vector<Sprite>& sprites, Graphics& graphics, GameObject& obj) {
    for (auto& sprite : sprites) {
        auto first_location = sprite.location;
        sprite.filename = (std::filesystem::current_path() / "assets" / sprite.filename).string();
        sprite.texture_id = graphics.get_texture_id(sprite.filename);
        sprite.shift = {-sprite.size.x / 2, -sprite.size.y};
        sprite.center = sprite.size / 2.0f;

        std::vector<Sprite> sprite_frames;
        for (int i = 0; i < sprite.number_of_frames; ++i) {
            sprite.location = {first_location.x + i * sprite.size.x, first_location.y};
            sprite_frames.push_back(sprite);
        }
        obj.sprites[sprite.name] = AnimatedSprite(sprite_frames, sprite.dt_per_frame);
    }
}

void AssetManager::get_game_object_details(const std::string &name, Graphics &graphics, GameObject &obj) {
    auto path_start = std::filesystem::current_path() / "assets";
    auto path = path_start/(name + ".json");

    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open file: " + path.string());
    }
    nlohmann::json json;
    file >> json;

    // get the object's sprites
    std::vector<Sprite> sprites_from_json = json.at("sprites").get<std::vector<Sprite>>();
    convert_sprites(sprites_from_json, graphics, obj);

    // get the object's physics
    Physics physics = json.at("physics").get<Physics>();
    obj.obj_physics = physics;
}