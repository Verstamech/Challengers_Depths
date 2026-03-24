#include "game_object.h"
#include "physics.h"
#include "fsm.h"
#include "input.h"

GameObject::GameObject(const Vec<float>& size, World& world, FSM* fsm, Input* input, Color color)
    : size{size}, fsm{fsm}, input{input}, color{color}, spd{4} {}

GameObject::~GameObject() {
    delete fsm;
    delete input;
}

void GameObject::update(World& world, double dt) {
    fsm->current_state->update(world, *this, dt);
    sprites[sprite_name].update(dt);
    sprites[sprite_name].flip(flip);
    set_sprite(sprite_name);
}

std::pair<Vec<float>, Color> GameObject::get_sprite() const {
    return {obj_physics.position, color};
}

void GameObject::set_sprite(const std::string &next_sprite) {
    // if the next sprite is a new sprite, reset current sprite
    if (next_sprite != sprite_name) {
        sprites[sprite_name].reset();

        auto itr = sprites.find(next_sprite);
        if (itr != sprites.end()) {
            sprite_name = next_sprite;
        }
        else {
            sprite_name = "idle"; // assume every GameObject has an idle sprite
        }
    }
    sprite = sprites[sprite_name].get_sprite();
}