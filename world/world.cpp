#include "world.h"
#include <algorithm>
#include "physics.h"
#include "game_object.h"
#include "fsm.h"
#include "states.h"
#include <SDL3/SDL_rect.h>
#include "vec.h"
#include "keyboard_input.h"
#include "level.h"
#include "audio.h"
#include "events.h"
#include <iostream>

World::World(const Level& level, Audio& audio, GameObject* player, Events events)
    : tilemap(level.width, level.height), audio{&audio}, player{player}, events{events}, quadtree{AABB{{level.width / 2.0f, level.height / 2.0f}, {level.width / 2.0f, level.height / 2.0f}}} {
    load_level(level);
}

World::~World() {
    for (auto obj : game_objects) {
        if (obj == player) continue;
        delete obj;
    }
}

void World::add_platform(float x, float y, float width, float height) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            tilemap(x+j, y+i) = Tile{};
        }
    }
}

bool World::collides(const Vec<float> &position) const {
    int x = std::floor(position.x);
    int y = std::floor(position.y);

    return tilemap(x, y).blocking;
}

void World::update(float dt) {
    // currently only updating player because we have no other game objects
    for (auto& obj : game_objects) {
        update_object(obj, dt);
        touch_tiles(*obj);
    }

    // update projectiles
    for (auto& projectile : projectiles) {
        update_object(projectile, dt);
    }

    // Check for collision with the player
    build_quadtree();
    std::vector<GameObject*> collides_with = quadtree.query_range(player->get_bounding_box());
    for (auto& obj : collides_with) {
        if (obj == player) continue;

        player->take_damage(obj->damage);
    }

    // check for collision with projectile and enemy
    for (auto& projectile : projectiles) {
        std::vector<GameObject*> p_collides_with = quadtree.query_range(projectile->get_bounding_box());
        for (auto& obj : p_collides_with) {
            if (obj == player) continue;
            obj->take_damage(projectile->damage);
            projectile->elapsed += projectile->lifetime;
        }
    }

    // std::partition puts objects that return TRUE at the beginning.
    // So we flip the logic: Keep alive objects at the front.
    auto itr = std::stable_partition(game_objects.begin(), game_objects.end(),
        [](GameObject* obj) { return obj->is_alive; }
    );

    // Now [itr, end) contains the original pointers to the dead objects
    std::for_each(itr, game_objects.end(), [](GameObject* p) { delete p; });
    game_objects.erase(itr, game_objects.end());

    // same for projectiles
    auto p_itr = std::stable_partition(projectiles.begin(), projectiles.end(),
        [](Projectile* projectile) { return projectile->elapsed <= projectile->lifetime; }   );

    std::for_each(p_itr, projectiles.end(), [](Projectile* p) { delete p; });
    projectiles.erase(p_itr, projectiles.end());

    // Check for player death
    if (!player->is_alive) {
        end_game = true;
        return;
    }
}

void World::update_object(GameObject *obj, float dt) {
    obj->update(*this, dt);
    auto position = obj->physics.position;
    auto velocity = obj->physics.velocity;
    auto acceleration = obj->physics.acceleration;

    // auto spd = obj->spd;
    auto dir = obj->dir;
    velocity.x = acceleration.x * dir;
    velocity.y += 0.5f * acceleration.y * dt;
    position += velocity * dt;

    velocity.x = std::clamp(velocity.x, -acceleration.x, acceleration.x);
    velocity.y = std::clamp(velocity.y, -obj->physics.terminal_velocity, obj->physics.terminal_velocity);

    // ... the code to update velocity and position

    // check for collisions in the world - x direction
    Vec<float> future_position{position.x, obj->physics.position.y};
    Vec<float> future_velocity{velocity.x, 0};
    move_to(future_position, obj->size, future_velocity);

    // now y direction after (maybe) moving in x
    future_velocity.y = velocity.y;
    future_position.y = position.y;
    move_to(future_position, obj->size, future_velocity);

    // update the obj position and velocity
    obj->physics.position = future_position;
    obj->physics.velocity = future_velocity;
}

void World::move_to(Vec<float>& position, const Vec<int>& size, Vec<float>& velocity) {
    float epsilon = 0.001f;
    auto left = position.x;
    auto right = position.x + size.x - epsilon;
    auto bottom = position.y;
    auto top = position.y + size.y - epsilon;

    // test for collisions on the bottom or top first
    if (collides({left, bottom}) && collides({right, bottom})) {
        position.y = std::ceil(position.y);
        velocity.y = 0;
    }
    else if (collides({left, top}) && collides({right, top})) {
        position.y = std::floor(position.y);
        velocity.y = 0;
    }

    // then test for collisions on the left and right sides
    if (collides({left, bottom}) && collides({left, top})) {
        position.x = std::ceil(position.x);
        velocity.x = 0;
    }
    else if (collides({right, bottom}) && collides({right, top})) {
        position.x = std::floor(position.x);
        velocity.x = 0;
    }

    // now test each corner
    float dx = std::ceil(position.x) - position.x;
    float dy = std::ceil(position.y) - position.y;
    // BOX'S BOTTOM LEFT TOUCHING TOP RIGHT OF AN OBJECT
    if (collides({left, bottom})) {
        if (dx > dy) {
            position.y = std::ceil(position.y);
            velocity.y = 0;
        }
        else {
            position.x = std::ceil(position.x);
            velocity.x = 0;
        }
    }
    // BOX'S TOP LEFT TOUCHING BOTTOM RIGHT OF AN OBJECT
    if (collides({left, top})) {
        dx = std::ceil(position.x) - position.x;
        dy = position.y - std::floor(position.y);

        if (dx > dy) {
            position.y = std::floor(position.y);
            velocity.y = 0;
        }
        else {
            position.x = std::ceil(position.x);
            velocity.x = 0;
        }
    }
    // BOX'S TOP RIGHT TOUCHING BOTTOM LEFT OF AN OBJECT
    if (collides({right, top})) {
        dx = position.x - std::floor(position.x);
        dy = position.y - std::floor(position.y);

        if (dx > dy) {
            position.y = std::floor(position.y);
            velocity.y = 0;
        }
        else {
            position.x = std::floor(position.x);
            velocity.x = 0;
        }
    }
    // BOX'S BOTTOM RIGHT TOUCHING TOP LEFT OF AN OBJECT
    if (collides({right, bottom})) {
        dx = position.x - std::floor(position.x);
        dy = std::ceil(position.y) - position.y;

        if (dx > dy) {
            position.y = std::ceil(position.y);
            velocity.y = 0;
        }
        else {
            position.x = std::floor(position.x);
            velocity.x = 0;
        }
    }

}

void World::load_level(const Level& level) {
    audio->load_sounds({});
    for (const auto& [pos, tile_id] : level.tile_locations) {
        tilemap(pos.x, pos.y) = level.tile_types.at(tile_id);
    }

    // get all enemies
    for (const auto& [pos, enemy_name] : level.enemy_locations) {
        auto enemy = new GameObject{enemy_name, nullptr, nullptr, {255, 0, 0, 255}};
        enemy->physics.position = pos;
        game_objects.push_back(enemy);
    }

    game_objects.push_back(player);
}

void World::touch_tiles(GameObject& obj) {
    float epsilon = 0.001f;

    const std::vector<Vec<float>> tiles {
            {obj.physics.position.x - epsilon, obj.physics.position.y},
            {obj.physics.position.x, obj.physics.position.y + obj.size.y + epsilon},
            {obj.physics.position.x + obj.size.x + epsilon, obj.physics.position.y},
            {obj.physics.position.x, obj.physics.position.y - epsilon}
    };

    for (const auto& p : tiles) {
        int x = static_cast<int>(std::floor(p.x));
        int y = static_cast<int>(std::floor(p.y));
        Tile& tile = tilemap(x, y);

        if (!tile.event_name.empty()) {
            auto itr = events.find(tile.event_name);
            if (itr == events.end()) {
                throw std::runtime_error("Cannot find event: " + tile.event_name);
            }
            itr->second->perform(*this, obj);
        }
    }
}

void World::build_quadtree() {
    quadtree.clear();

    for (auto obj : game_objects) {
        quadtree.insert(obj);
    }
}