#include "game.h"

#include "asset_manager.h"
#include "fsm.h"
#include "input.h"

Game::Game(std::string title, int width, int height)
    : graphics{title, width, height}, camera{graphics, 64}, dt{1.0 / 60.0}, performance_frequency{SDL_GetPerformanceFrequency()}, prev_counter{SDL_GetPerformanceCounter()}, lag{0.0} {

    // Load the first level
    Level level{"level_1"};
    AssetManager::get_level_details(graphics, level);

    // create the world for the first level
    world = new World(level, audio);

    // Give player its assets then put it in the correct state
    player = std::unique_ptr<GameObject>(world->create_player(level));

    AssetManager::get_game_object_details("player", graphics, *player);
    // use the spawn location's position
    player->obj_physics.position = {static_cast<float>(level.player_spawn_location.x), static_cast<float>(level.player_spawn_location.y)};
    player->fsm->current_state->on_enter(*world, *player);

    camera.set_location(player->obj_physics.position);
    audio.play_sounds("background", true);
}

void Game::handle_event(SDL_Event* event) {
    player->input->collect_discrete_event(event);
}

void Game::input() {
    player->input->get_input();
    camera.handle_input();
}

void Game::update() {
    Uint64 now = SDL_GetPerformanceCounter();
    lag += (now - prev_counter) / (float) performance_frequency;
    prev_counter = now;
    while (lag >= dt) {
        player->input->handle_input(*world, *player);
        player->update(*world, dt);
        world->update(dt);

        // put the camera slightly ahead of the player
        float L = length(player->obj_physics.velocity);
        Vec displacement = 2.0f * player->obj_physics.velocity / (1.0f + L);
        camera.update(player->obj_physics.position + displacement, dt);
        lag -= dt;
    }
}

void Game::render() {
    // clear
    graphics.clear();

    // draw the world
    camera.render(world->tilemap);

    // draw the player
    camera.render(*player);

    // update
    graphics.update();
}