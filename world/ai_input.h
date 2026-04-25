#pragma once
#include "input.h"

class AIInput : public Input {
    public:
    void get_input() override;
    void handle_input(World& world, GameObject& obj) override;
};