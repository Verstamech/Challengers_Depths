#pragma once
#include "input.h"

class KeyboardInput : public Input {
public:
    Action* collect_discrete_event(SDL_Event *event) override;
    void get_input() override;
    void handle_input(World &world, GameObject &obj) override;
};