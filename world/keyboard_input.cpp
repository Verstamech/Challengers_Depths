#include "keyboard_input.h"
#include "game_object.h"
#include "fsm.h"

void KeyboardInput::get_input() {
    if (next_action_type == ActionType::Jump || next_action_type == ActionType::AttackAll) return;

    const bool *key_states = SDL_GetKeyboardState(NULL);

    if (key_states[SDL_SCANCODE_LEFT] || key_states[SDL_SCANCODE_A]) {
        next_action_type = ActionType::MoveLeft;
    }
    else if (key_states[SDL_SCANCODE_RIGHT] || key_states[SDL_SCANCODE_D]) {
        next_action_type = ActionType::MoveRight;
    }
}

void KeyboardInput::handle_input(World &world, GameObject &obj) {
    Action* action = obj.fsm->current_state->input(world, obj, next_action_type);

    // consume the action
    next_action_type = ActionType::None;
    if (action != nullptr) {
        action->perform(world, obj);
        delete action;
    }
}

Action* KeyboardInput::collect_discrete_event(SDL_Event *event) {
    if (event->type == SDL_EVENT_KEY_DOWN && event->key.repeat == 0) {
        if (event->key.scancode == SDL_SCANCODE_SPACE || event->key.scancode == SDL_SCANCODE_X) {
            next_action_type = ActionType::Jump;
        }
        if (event->key.scancode == SDL_SCANCODE_H) {
            next_action_type = ActionType::AttackAll;
        }
        if (event->key.scancode == SDL_SCANCODE_F) {
            return new ShootFireball();
        }
    }
    
    return nullptr;
}