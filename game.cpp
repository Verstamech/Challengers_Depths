#include "game.h"

#include "asset_manager.h"
#include "fsm.h"

Game::Game(std::string title, int width, int height)
    : graphics{title, width, height}, world{31, 11}, camera{graphics, 64}, dt{1.0 / 60.0}, lag{0.0}, performance_frequency{SDL_GetPerformanceFrequency()}, prev_counter{SDL_GetPerformanceCounter()} {

    // Load the first "level"
    // boundary walls
    world.add_platform(0, 0, 30, 1);
    world.add_platform(0, 0, 1, 10);
    world.add_platform(30, 0, 1, 10);
    world.add_platform(0, 10, 31, 1);

    // platforms
    world.add_platform(3, 7, 4, 1);
    world.add_platform(13, 4, 6, 1);

    player = world.create_player();
    AssetManager::get_game_object_details("player", graphics, *player);
    player->fsm->current_state->on_enter(world, *player);

    camera.set_location(player->obj_physics.position);
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
        player->input->handle_input(world, *player);
        player->update(world, dt);
        world.update(dt);

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
    camera.render(world.tilemap);

    // draw the player
    camera.render(*player);

    // update
    graphics.update();
}