#pragma once
#include "physics.h"
#include "tilemap.h"
#include "vec.h"
#include "toggle.h"

class Graphics;
class Color;
class Sprite;
class GameObject;

class Camera {
public:
    Camera(Graphics& graphics, float tilesize);

    void handle_input();
    void update(const Vec<float>& new_location, float dt);
    Vec<float> world_to_screen(const Vec<float>& world_position) const;
    void set_location(const Vec<float>& new_location);

    void render(const Vec<float>& position, const Color& color, bool filled = true) const;
    void render(const Tilemap& tilemap) const;
    void render(const Vec<float>& position, const Sprite& sprite) const;
    void render(const GameObject& obj) const;

private:
    Graphics& graphics;
    float tilesize;
    Vec<float> location;
    Toggle grid_toggle;
    Physics physics;

    void calculate_visible_tiles();
    Vec<int> visible_min, visible_max;
    Vec<float> goal, velocity, acceleration;
};