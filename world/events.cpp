#include "events.h"

#include <iostream>

#include "world.h"
#include "game_object.h"

void NextLevel::perform(World &world, GameObject &obj) {
    if (&obj != world.player) return;
    world.end_level = true;
}
