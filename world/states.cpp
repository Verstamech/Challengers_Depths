#include "states.h"
#include "action.h"
#include "game_object.h"

// Helper function
bool on_platform(World& world ,GameObject& obj) {
    constexpr float epsilon = 1e-4;
    Vec<float> left_foot{obj.physics.position.x + epsilon, obj.physics.position.y - epsilon};
    Vec<float> right_foot{obj.physics.position.x + obj.size.x - epsilon, obj.physics.position.y - epsilon};
    return world.collides(left_foot) || world.collides(right_foot);
}

// Standing
void Standing::on_enter(World &world, GameObject& obj) {
    obj.color = {0, 0, 255, 255};
    obj.set_sprite("idle");
    obj.dir = 0;
}

Action *Standing::input(World& world, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::Jump && on_platform(world, obj)) {
        obj.fsm->transition(Transition::Jump, world, obj);
        return new Jump();
    }

    if (action_type == ActionType::MoveRight) {
        obj.fsm->transition(Transition::Move, world, obj);
        return new MoveRight();
    }
    if (action_type == ActionType::MoveLeft) {
        obj.fsm->transition(Transition::Move, world, obj);
        return new MoveLeft();
    }

    return nullptr;
}

// Airborne
void Airborne::on_enter(World &world, GameObject &obj) {
    elapsed = cooldown;
    obj.set_sprite("jumping");
    obj.color = {255, 255, 0, 255};
}

void Airborne::update(World& world, GameObject& obj, double dt) {
    elapsed -= dt;
    if (elapsed <= 0 && on_platform(world, obj)) {
        obj.fsm->transition(Transition::Stop, world, obj);
    }
}

Action *Airborne::input(World &world, GameObject & obj, ActionType action_type) {
    if (action_type == ActionType::MoveRight) {
        obj.fsm->transition(Transition::Move, world, obj);
        return new MoveRight();
    }

    if (action_type == ActionType::MoveLeft) {
        obj.fsm->transition(Transition::Move, world, obj);
        return new MoveLeft();
    }
    obj.dir = 0;

    return nullptr;
}

// Running
void Running::on_enter(World &, GameObject & obj) {
    obj.color = {0, 255, 0, 255};
    obj.set_sprite("walking");
}

void Running::update(World& world, GameObject& obj, double dt) {
    if (!on_platform(world, obj)) {
        obj.jump_length += dt;
        obj.set_sprite("jumping");
    }
    else {
        obj.set_sprite("walking");
    }
}

Action *Running::input(World& world, GameObject& obj, ActionType action_type) {
    if (action_type == ActionType::Jump && on_platform(world, obj)) {
        obj.fsm->transition(Transition::Jump, world, obj);
        return new Jump();
    }

    if (action_type == ActionType::MoveRight) {
        obj.fsm->transition(Transition::Move, world, obj);
        return new MoveRight();
    }

    if (action_type == ActionType::MoveLeft) {
        obj.fsm->transition(Transition::Move, world, obj);
        return new MoveLeft();
    }

    if (action_type == ActionType::None) {
        if (on_platform(world, obj)) {
            obj.fsm->transition(Transition::Stop, world, obj);
        }
        else {
            obj.fsm->transition(Transition::Stop_Midair, world, obj);
        }
    }

    return nullptr;
}
