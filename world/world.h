#pragma once
#include <memory>

#include "physics.h"
#include "tilemap.h"
#include "vec.h"
#include "keyboard_input.h"
#include <map>
#include "events.h"

class GameObject;
class Level;
class Audio;

class World {
public:
    World(const Level& level, Audio& audio, GameObject* player, Events events);

    void add_platform(float x, float y, float width, float height);
    bool collides(const Vec<float>& position) const;
    void update(float dt);
    void load_level(const Level& level);

    void move_to(Vec<float>& position, const Vec<float>& size, Vec<float>& velocity);

    Tilemap tilemap;
    bool end_level{false};

private:
    GameObject* player;
    Physics physics;
    Audio* audio;
    Events events;

    void touch_tiles(GameObject& obj);
};