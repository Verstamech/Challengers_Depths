#include "camera.h"
#include "graphics.h"
#include "physics.h"
#include "game_object.h"
#include "world.h"

Camera::Camera(Graphics &graphics, float tilesize)
    : graphics{graphics}, tilesize{tilesize} {
    calculate_visible_tiles();
    physics.damping = 0.9f;
}

void Camera::calculate_visible_tiles() {
    Vec<int> num_tiles = Vec{graphics.width, graphics.height} / (2 * static_cast<int>(tilesize)) + Vec{1, 1};
    Vec<int> center{static_cast<int>(location.x), static_cast<int>(location.y)};
    visible_max = center + num_tiles;
    visible_min = center - num_tiles;
}

Vec<float> Camera::world_to_screen(const Vec<float>& world_position) const {
    // world coordinates (pos y is up) -> screen coords (pos y is down)
    Vec<float> pixel = (world_position - location) * static_cast<float>(tilesize);

    // shift to center
    pixel += Vec<float>{graphics.width / 2.0f, graphics.height / 2.0f};

    // flip y axis
    pixel.y = graphics.height - pixel.y;

    return pixel;
}

void Camera::handle_input() {
    const bool *key_states = SDL_GetKeyboardState(NULL);

    if (key_states[SDL_SCANCODE_G]) {
        grid_toggle.flip();
    }
}

void Camera::update(const Vec<float> &new_location, float dt) {
    goal = new_location;
    acceleration = (goal - location) * 10.0f;

    velocity += 0.5f * acceleration * dt;
    location += velocity * dt;
    velocity += 0.5f * acceleration * dt;
    velocity *= {physics.damping, physics.damping};

    calculate_visible_tiles();
}

void Camera::set_location(const Vec<float>& new_location) {
    location = new_location;
    calculate_visible_tiles();
}

void Camera::render(const Vec<float> &position, const Color &color, Vec<int> size, bool filled) const {
    Vec<float> pixel = world_to_screen(position);

    // move from center to bottom left
    pixel.x -= tilesize / 2.0f;
    pixel.y += tilesize / 2.0f;

    // offset by full height so feet stay at bottom anchor
    float screen_w = tilesize * size.x;
    float screen_h = tilesize * size.y;


    SDL_FRect rect{pixel.x, pixel.y - screen_h, screen_w, screen_h};
    graphics.draw(rect, color, filled);
}

void Camera::render(const Tilemap& tilemap) const {
    int xmin = std::max(0, visible_min.x);
    int ymin = std::max(0, visible_min.y);
    int xmax = std::min(visible_max.x, tilemap.width - 1);
    int ymax = std::min(visible_max.y, tilemap.height - 1);

    // draw tiles
    for (int y = ymin; y <= ymax; ++y) {
        for (int x = xmin; x <= xmax; ++x) {
            const Tile& tile = tilemap(x, y);
            Vec<float> position{static_cast<float>(x), static_cast<float>(y)};

            render(position, tile.sprite, 1);

            if (grid_toggle.on) {
                render(position, Color{0, 0, 0, 255}, false);
            }
        }
    }
}

void Camera::render(const Vec<float> &position, const Sprite &sprite, Vec<int> size, bool flash) const {
    Vec<float> pixel = world_to_screen(position);
    float x_offset = (size.x - 1) * (tilesize / 2.0f);
    pixel.x += x_offset;
    pixel.y += tilesize / 2.0f;
    graphics.draw_sprite(pixel, sprite, flash);
}

void Camera::render(const GameObject &obj) const {
    if (grid_toggle.on) { // FOR DEBUGGING PURPOSES
        render(obj.physics.position, obj.color, obj.size);
    }
    render(obj.physics.position, obj.sprite, obj.size, obj.flash_sprite());
}

void Camera::render_game_over() {
    SDL_FRect full_screen{0.0f, 0.0f, static_cast<float>(graphics.width), static_cast<float>(graphics.height)};
    graphics.draw(full_screen, Color{0,0,0, 180}, true);
}