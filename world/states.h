#pragma once
#include "fsm.h"
#include "world.h"

class Standing: public State {
public:
    void on_enter(World &, GameObject &) override;
    Action* input(World &, GameObject &, ActionType) override;
};

class Airborne: public State {
public:
    void on_enter(World &, GameObject &) override;
    void update(World &, GameObject &, double dt) override;
    Action* input(World &, GameObject &, ActionType) override;

    const double cooldown = 0.1;
    double elapsed = 0;
};

class Running: public State {
public:
    void on_enter(World&, GameObject&) override;
    void update(World&, GameObject&, double dt) override;
    Action* input(World&, GameObject&, ActionType) override;
};

class Patrolling : public Running {
public:
    void on_enter(World&, GameObject&) override;
    Action* input(World&, GameObject&, ActionType) override;
    void update(World&, GameObject&, double dt) override;

    double elapsed{0.0};
    double cooldown{0.0};
};

class AttackAllEnemies : public State {
public:
    virtual void on_enter(World&, GameObject&) override;
    virtual void update(World&, GameObject&, double dt) override;

    double elapsed = 0;
    double cooldown = 2;
};