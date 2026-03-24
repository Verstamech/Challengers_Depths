#pragma once
#include <memory>

#include "physics.h"
#include "tilemap.h"
#include "vec.h"
#include "keyboard_input.h"

class GameObject;

class World {
public:
    World(int width, int height);

    void add_platform(float x, float y, float width, float height);
    bool collides(const Vec<float>& position) const;
    GameObject* create_player();
    void update(float dt);

    void move_to(Vec<float>& position, const Vec<float>& size, Vec<float>& velocity);

    Tilemap tilemap;

private:
    std::unique_ptr<GameObject> player;
    Physics physics;
};