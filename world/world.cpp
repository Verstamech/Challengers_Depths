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

World::World(const Level& level, Audio& audio, GameObject* player, Events events)
    : tilemap(level.width, level.height), audio{&audio}, player{player}, events{events} {
    load_level(level);
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
    auto position = player->physics.position;
    auto velocity = player->physics.velocity;
    auto acceleration = player->physics.acceleration;

    auto spd = player->spd;
    auto dir = player->dir;
    velocity.x = spd * dir;
    velocity.y += 0.5f * acceleration.y * dt;
    position += velocity * dt;

    velocity.x = std::clamp(velocity.x, -player->physics.terminal_velocity, player->physics.terminal_velocity);
    velocity.y = std::clamp(velocity.y, -player->physics.terminal_velocity, player->physics.terminal_velocity);

    // ... the code to update velocity and position

    // check for collisions in the world - x direction
    Vec<float> future_position{position.x, player->physics.position.y};
    Vec<float> future_velocity{velocity.x, 0};
    move_to(future_position, player->size, future_velocity);

    // now y direction after (maybe) moving in x
    future_velocity.y = velocity.y;
    future_position.y = position.y;
    move_to(future_position, player->size, future_velocity);

    // update the player position and velocity
    player->physics.position = future_position;
    player->physics.velocity = future_velocity;

    touch_tiles(*player);
}

void World::move_to(Vec<float>& position, const Vec<float>& size, Vec<float>& velocity) {
    // test for collisions on the bottom or top first
    if (collides({position.x, position.y}) && collides({position.x + size.x, position.y})) {
        position.y = std::ceil(position.y);
        velocity.y = 0;
    }
    else if (collides({position.x, position.y + size.y}) && collides({position.x + size.x, position.y + size.y})) {
        position.y = std::floor(position.y);
        velocity.y = 0;
    }

    // then test for collisions on the left and right sides
    if (collides({position.x, position.y}) && collides({position.x, position.y + size.y})) {
        position.x = std::ceil(position.x);
        velocity.x = 0;
    }
    else if (collides({position.x + size.x, position.y}) && collides({position.x + size.x, position.y + size.y})) {
        position.x = std::floor(position.x);
        velocity.x = 0;
    }

    // now test each corner
    float dx = std::ceil(position.x) - position.x;
    float dy = std::ceil(position.y) - position.y;
    // BOX'S BOTTOM LEFT TOUCHING TOP RIGHT OF AN OBJECT
    if (collides({position.x, position.y})) {
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
    else if (collides({position.x, position.y + size.y})) {
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
    else if (collides({position.x + size.x, position.y + size.y})) {
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
    else if (collides({position.x + size.x, position.y})) {
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
}

void World::touch_tiles(GameObject &obj) {
    int x = std::floor(obj.physics.position.x);
    int y = std::floor(obj.physics.position.y);
    const std::vector<Vec<int>> displacements{{0,0}, {static_cast<int>(obj.size.x), 0}, {0, static_cast<int>(obj.size.y)}, {static_cast<int>(obj.size.x), static_cast<int>(obj.size.y)}};

    for (const auto& displacement : displacements) {
        Tile& tile = tilemap(x + displacement.x, y + displacement.y);

        if (!tile.event_name.empty()) {
            auto itr = events.find(tile.event_name);
            if (itr == events.end()) {
                throw std::runtime_error("Cannot find event: " + tile.event_name);
            }
            itr->second->perform(*this, obj);
        }
    }
}