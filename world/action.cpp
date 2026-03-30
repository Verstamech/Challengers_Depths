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