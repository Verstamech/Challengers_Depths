#include "level_designer.h"
#include "asset_manager.h"

const int TILESIZE = 64;
const int VISIBLE_MAP_WIDTH = 14;
const int VISIBLE_MAP_HEIGHT = 12;
constexpr int COLUMNS = 5;
constexpr float PADDING = 16.0f;

LevelDesigner::LevelDesigner(const std::string &level_name, int width, int height)
    : graphics{"Level Designer", 1280, 720}, tilemap{width, height}, level{level_name},
    dt{0.1}, performance_frequency{SDL_GetPerformanceFrequency()}, prev_counter{SDL_GetPerformanceCounter()}, lag{0.0},
    display_rect{0.0f, 0.0f, graphics.width*(2.0f/3.0f), static_cast<float>(graphics.height)},
    tiles_rect{graphics.width*(2.0f/3.0f), 0.0f, graphics.width*(1.0f/3.0f), static_cast<float>(graphics.height)}{
    update_title();

    AssetManager::get_level_details(graphics, level);

    // set up palette ids for tile selection
    for (auto& [id, tile] : level.tile_types) {
        palette_ids.push_back(id);
        tile.sprite.shift = {0,0};
    }

    // update the tilemap
    for (const auto& [pos, tile_id] : level.tile_locations) {
        tilemap(pos.x, pos.y) = level.tile_types[tile_id];
    }
}

void LevelDesigner::update_title() {
    std::string title = "Level Designer";

    if (selected_tile.x >= 0 && selected_tile.y >= 0) {
        title += " - selected: (" + std::to_string(selected_tile.x) + ", " + std::to_string(selected_tile.y) + ")";
    }

    graphics.set_title(title);
}

void LevelDesigner::handle_event(SDL_Event *event) {
    // use mouse to select a tile position
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT) {
        float mouse_x = event->button.x;
        float mouse_y = event->button.y;

        if (mouse_x > display_rect.x && mouse_y > display_rect.y && mouse_x < display_rect.x + display_rect.w && mouse_y < display_rect.y + display_rect.h) {
            int grid_x = static_cast<int>((mouse_x - display_rect.x) / TILESIZE);
            int grid_y = VISIBLE_MAP_HEIGHT - 1 - static_cast<int>((mouse_y - display_rect.y) / TILESIZE);

            int tilemap_x = x_min + grid_x;
            int tilemap_y = y_min + grid_y;

            if (tilemap_x >= 0 && tilemap_x < tilemap.width && tilemap_y >= 0  && tilemap_y < tilemap.height) {
                selected_tile = {tilemap_x, tilemap_y};
            }
            else {
                selected_tile = {-1, -1};
            }
            update_title();
            update_tilemap();
        }
        else if (mouse_x > tiles_rect.x && mouse_y > tiles_rect.y && mouse_x < tiles_rect.x + tiles_rect.w && mouse_y < tiles_rect.y + tiles_rect.h) {
            float x_pos = mouse_x - tiles_rect.x - PADDING;
            float y_pos = mouse_y - tiles_rect.y - PADDING;

            if (x_pos >= 0 && y_pos >= 0) {
                int col = x_pos / (TILESIZE + PADDING);
                int row = y_pos / (TILESIZE + PADDING);
                int index = row * COLUMNS + col;
                selected_palette_id = index < palette_ids.size() ? palette_ids.at(index) : "";
                update_tilemap();
            }
        }
    }
}

void LevelDesigner::input() {
    Uint64 now = SDL_GetPerformanceCounter();
    lag += ((now - prev_counter) / (double)performance_frequency);
    prev_counter = now;
    const bool* keys = SDL_GetKeyboardState(nullptr);

    if ((keys[SDL_SCANCODE_DELETE] || keys[SDL_SCANCODE_BACKSPACE]) && selected_tile.x >= 0 && selected_tile.y >= 0) {
        tilemap(selected_tile.x, selected_tile.y) = Tile{};
    }
    // RGUI & LGUI for CMD on mac
    if (keys[SDL_SCANCODE_S] && (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL] || keys[SDL_SCANCODE_LGUI] || keys[SDL_SCANCODE_RGUI])) {
        save();
    }
    if (keys[SDL_SCANCODE_P]) {
        place_player();
    }

    // timer for scrolling
    if (lag < dt) {
        return;
    }
    lag = 0;
    if (keys[SDL_SCANCODE_LEFT]) {
        x_min--;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        x_min++;
    }
    else if (keys[SDL_SCANCODE_UP]) {
        y_min++;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        y_min--;
    }
}

void LevelDesigner::render() {
    graphics.clear();

    // draw the main display
    graphics.draw(display_rect, Color{30, 30, 30, 255});

    // draw the grid and tilemap
    for (int y = 0; y < VISIBLE_MAP_HEIGHT; ++y) {
        for (int x = 0; x < VISIBLE_MAP_WIDTH; ++x) {
            int tilemap_x = x_min + x;
            int tilemap_y = y_min + y;
            // grid
            if (tilemap_x >= 0 && tilemap_x < tilemap.width && tilemap_y >= 0  && tilemap_y < tilemap.height) {
                float screen_x = display_rect.x + x * TILESIZE;
                float screen_y = display_rect.y + (VISIBLE_MAP_HEIGHT - 1 - y) * TILESIZE;

                graphics.draw_sprite({screen_x, screen_y}, tilemap(tilemap_x, tilemap_y).sprite);
                SDL_FRect rect{screen_x, screen_y, static_cast<float>(TILESIZE), static_cast<float>(TILESIZE)};
                Color color = selected_tile == Vec<int>{tilemap_x, tilemap_y} ? Color{255, 255, 0, 255} : Color{0, 0, 0, 255};
                graphics.draw(rect, color, false);

                // render player location as translucent purple
                if (level.player_spawn_location.x == tilemap_x && level.player_spawn_location.y == tilemap_y) {
                    graphics.draw(rect, {255, 0, 255, 100}, true);
                }
            }
        }
    }


    // draw the tiles display
    graphics.draw(tiles_rect, Color{50, 50, 60, 255});
    draw_tile_display();

    // render
    graphics.update();
}

void LevelDesigner::draw_tile_display() {
    for (int i = 0; i < palette_ids.size(); ++i) {
        int col = i % COLUMNS;
        int row = i / COLUMNS;

        float x = tiles_rect.x + PADDING + col * (TILESIZE + PADDING);
        float y = tiles_rect.y + PADDING + row * (TILESIZE + PADDING);

        const std::string id = palette_ids.at(i);
        Tile& tile = level.tile_types.at(id);
        //tile.sprite.shift = {0,0};
        graphics.draw_sprite({static_cast<float>(x), static_cast<float>(y)}, tile.sprite);

        // highlight
        if (selected_palette_id == palette_ids.at(i)) {
            SDL_FRect spot{x, y, TILESIZE, TILESIZE};
            graphics.draw(spot, Color{255, 255, 0, 255}, false);
        }
    }
}

void LevelDesigner::update_tilemap() {
    if (selected_tile.x >= 0 && selected_tile.y >= 0 && !selected_palette_id.empty()) {
        tilemap(selected_tile.x, selected_tile.y) = level.tile_types[selected_palette_id];
    }
}

void LevelDesigner::save() {
    level.tile_locations.clear();
    for (int y = 0; y < tilemap.height; ++y) {
        for (int x = 0; x < tilemap.width; ++x) {
            if (tilemap(x, y).sprite.texture_id > -1) {
                level.tile_locations[{x,y}] = tilemap(x,y).id;
            }
        }
    }
    AssetManager::update_level_details(level);
}

void LevelDesigner::place_player() {
    level.player_spawn_location = selected_tile;
}