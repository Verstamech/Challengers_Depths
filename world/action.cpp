#include "action.h"

#include "audio.h"
#include "game_object.h"
#include "world.h"

void Jump::perform(World& world, GameObject& obj) {
    obj.physics.velocity.y = obj.physics.jump_velocity;
    world.audio->play_sounds("land");
}

void MoveRight::perform(World&, GameObject& obj) {
    if (obj.obj_name == "player") {
        obj.flip = false;
    }
    else {
        obj.flip = true;
    }
    obj.dir = 1;
}
void MoveLeft::perform(World&, GameObject& obj) {
    if (obj.obj_name == "player") {
        obj.flip = true;
    }
    else {
        obj.flip = false;
    }
    obj.dir = -1;
}

void ShootTrident::perform(World &world, GameObject &obj) {
    auto trident = dynamic_cast<Projectile*>(world.available_items["trident"]());
    trident->physics.position = obj.physics.position;
    trident->flip = world.player->flip;
    if (trident->flip) {
        trident->dir = -1;
    }
    else {
        trident->dir = 1;
    }
    world.projectiles.push_back(trident);

    world.audio->play_sounds("trident");
}