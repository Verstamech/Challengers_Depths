#pragma once
#include <utility>
#include "vec.h"
#include "graphics.h"
#include "physics.h"
#include <map>
#include "animated_sprite.h"

class World;
class FSM;
class Input;

using Sprites = std::map<std::string, AnimatedSprite>;

class GameObject {
public:
    GameObject(const Vec<float>& spawn_location, const Vec<float>& size, World& world, FSM* fsm, Input* input, Color color);
    ~GameObject();

    void update(World& world, double dt);

    std::pair<Vec<float>, Color> get_sprite() const;
    void set_sprite(const std::string& next_sprite);

    // GameObject Data
    Physics obj_physics;
    bool flip = false;
    Vec<float> size;
    FSM* fsm;
    Input* input;
    Color color;
    Sprites sprites; // Collection of sprites to iterate through
    Sprite sprite; // The sprite being displayed currently
    std::string sprite_name; // The name of the sprite being displayed

    float spd;
    float dir;
    double jump_length = 0;
};
