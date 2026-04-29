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
class AABB;

using Sprites = std::map<std::string, AnimatedSprite>;

class GameObject {
public:
    GameObject(std::string name, FSM* fsm, Input* input, Color color = {255, 0, 0, 255});
    ~GameObject();

    void virtual update(World& world, double dt);

    std::pair<Vec<float>, Color> get_sprite() const;
    void set_sprite(const std::string& next_sprite);

    AABB get_bounding_box();

    void take_damage(int attack_damage);
    bool flash_sprite() const;

    // GameObject Data
    std::string obj_name;
    Physics physics;
    bool flip = false;
    Vec<float> size;
    FSM* fsm;
    Input* input;
    Color color;
    Sprites sprites; // Collection of sprites to iterate through
    Sprite sprite; // The sprite being displayed currently
    std::string sprite_name; // The name of the sprite being displayed

    float dir = 0;
    double jump_length = 0;

    // combat stuff
    int health;
    int max_health;
    int damage;
    bool is_alive{true};

    double inv{0.0};
};
