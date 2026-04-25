#pragma once
#include <map>

class World;
class GameObject;
class Action;
enum class ActionType;
class State;

enum class StateType {
    Standing,
    Airborne,
    Running,
    AttackAll,
    Patrolling
};
enum class Transition {
    Jump,
    Stop,
    Stop_Midair,
    Move,
    AttackAll
};

using Transitions = std::map<std::pair<StateType, Transition>, StateType>;
using States = std::map<StateType, State*>;

class FSM {
public:
    FSM(Transitions transitions, States states, StateType start);
    virtual ~FSM();

    void transition(Transition t, World& world, GameObject& obj);

    // FSM data
    Transitions transitions;
    States states;
    StateType current_state_type;
    State* current_state;
};

class State {
public:
    virtual ~State() = default;

    virtual void on_enter(World&, GameObject&) {}
    virtual void on_exit(World&, GameObject&) {}

    virtual Action* input(World&, GameObject&, ActionType) {
        return nullptr;
    }
    virtual void update(World&, GameObject&, [[maybe_unused]] double dt) {}
};