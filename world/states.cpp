#include "states.h"
#include "action.h"
#include "audio.h"
#include "game_object.h"
#include "random.h"
#include "world.h"

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
    if (action_type == ActionType::AttackAll) {
        obj.fsm->transition(Transition::AttackAll, world, obj);
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

// Patrolling
void Patrolling::on_enter(World &world, GameObject &obj) {
    obj.set_sprite("idle");
    // set cooldown to a random amount of time, 3-10 seconds
    elapsed = 0;
    cooldown = randint(3, 10);
    Running::on_enter(world, obj);
}

Action *Patrolling::input(World &world, GameObject &obj, ActionType action_type) {
    Vec<float> player_pos = world.player->physics.position;
    Vec<float> obj_pos = obj.physics.position;

    if (std::floor(player_pos.y) == std::floor(obj_pos.y)) {
        obj.fsm->transition(Transition::Spotted, world, obj);
        return nullptr;
    }
    else if (elapsed >= cooldown) {
        return Running::input(world, obj, ActionType::None);
    }
    return Running::input(world, obj, action_type);
}

void Patrolling::update(World &world, GameObject &obj, double dt) {
    elapsed += dt;
}

void Aggro::on_enter(World& world, GameObject& obj) {
    obj.set_sprite("aggro");
    world.audio->play_sounds("spearfish_aggro");

    if (world.player->physics.position.x >= obj.physics.position.x) {
        obj.flip = true;
    }
    else {
        obj.flip = false;
    }
    elapsed = 0.0;

    original_acceleration = obj.physics.acceleration.x;
    obj.physics.acceleration.x = 0.0;
}

void Aggro::update(World&, GameObject&, double dt) {
    elapsed += dt;
}

Action *Aggro::input(World& world, GameObject& obj, ActionType) {
    if (elapsed >= 1.0) {
        obj.physics.acceleration.x = original_acceleration;
        obj.fsm->transition(Transition::Charge, world, obj);
    }
    return nullptr;
}

void Charge::on_enter(World&, GameObject& obj) {
    original_acceleration = obj.physics.acceleration.x;
    obj.physics.acceleration.x = obj.physics.walk_acceleration;
}

Action* Charge::input(World& world, GameObject& obj, ActionType) {
    float epsilon = 0.001f;

    // check for colliding with left wall
    if (world.collides({obj.physics.position.x - epsilon, obj.physics.position.y + obj.size.y - epsilon}) || world.collides({obj.physics.position.x + obj.size.x + epsilon, obj.physics.position.y + obj.size.y - epsilon})) {

        obj.physics.acceleration.x = original_acceleration;
        obj.fsm->transition(Transition::Stop_Midair, world, obj);
    }
    return nullptr;
}

// AttackAll
void AttackAllEnemies::on_enter(World& world, GameObject& obj) {
    obj.color = {255, 100, 0, 255};
    for (auto& enemy : world.game_objects) {
        if (enemy == world.player) continue;
        enemy->take_damage(obj.damage);
    }
    elapsed = 0;
}

void AttackAllEnemies::update(World &world, GameObject &obj, double dt) {
    elapsed += dt;
    if (elapsed >= cooldown) {
        obj.fsm->transition(Transition::Stop, world, obj);
    }
}