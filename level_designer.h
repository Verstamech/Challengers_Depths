#pragma once

#include "graphics.h"
#include "camera.h"
#include "level.h"
#include "vec.h"
#include "level.h"

class LevelDesigner {
public:
    LevelDesigner(const std::string& level_name, int width, int height);
    void handle_event(SDL_Event* event);
    void input();
    void render();
    void save();

private:
    Graphics graphics;
    Tilemap tilemap;
    Level level;

    Vec<int> selected_tile{-1, -1};
    int y_min{0};
    int x_min{0};
    std::vector<std::string> palette_ids;
    std::string selected_palette_id;

    // timing for scrolling
    double dt;
    Uint64 performance_frequency;
    Uint64 prev_counter;
    double lag;

    // UI pieces
    SDL_FRect display_rect;
    SDL_FRect tiles_rect;

    // helper functions
    void update_title();
    void draw_tile_display();
    void update_tilemap();
    void place_player();
};