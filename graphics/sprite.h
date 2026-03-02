#pragma once

#include "vec.h"

struct Sprite {
    int texture_id{-1};
    std::string filename;
    Vec<float> location{0, 0};
    Vec<float> size{0, 0};
    float scale{1};
    Vec<float> shift{0,0};
    Vec<float> center{0, 0};
    double angle{0.0};
    bool flip{false};
};