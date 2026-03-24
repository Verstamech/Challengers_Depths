#pragma once
#include <vector>
#include "sprite.h"

class AnimatedSprite {
public:
    AnimatedSprite();
    AnimatedSprite(const std::vector<Sprite>& sprites, double dt_per_frame, int starting_frame = 0);

    void flip(bool flip);
    void update(double dt);
    void reset();
    Sprite get_sprite();
    int number_of_frames();

private:
    std::vector<Sprite> sprites;
    double dt_per_frame{-1.0};
    double time{0};
    int current_frame{0};
};