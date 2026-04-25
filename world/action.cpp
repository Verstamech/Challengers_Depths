#include "action.h"
#include "game_object.h"
#include "world.h"

void Jump::perform(World&, GameObject& obj) {
    obj.physics.velocity.y = obj.physics.jump_velocity;
}

void MoveRight::perform(World&, GameObject& obj) {
    obj.flip = false;
    obj.dir = 1;
}
void MoveLeft::perform(World&, GameObject& obj) {
    obj.flip = true;
    obj.dir = -1;
}

void ShootFireball::perform(World &world, GameObject &obj) {
    auto fireball = dynamic_cast<Projectile*>(world.available_items["fireball"]());
    fireball->physics.position = obj.physics.position;
    world.projectiles.push_back(fireball);
}