#pragma once

enum class ActionType {
    None,
    Jump,
    MoveLeft,
    MoveRight,
    AttackAll
};

class World;
class GameObject;

class Action {
public:
    virtual ~Action() = default;
    virtual void perform(World& world, GameObject& obj) = 0;
};

class Jump : public Action {
    void perform(World&, GameObject& obj) override;
};

class MoveRight: public Action {
    void perform(World&, GameObject& obj) override;
};
class MoveLeft: public Action {
    void perform(World&, GameObject& obj) override;
};

class AttackAll : public Action {
    void perform(World&, GameObject& obj) override;
};

class ShootFireball : public Action {
    void perform(World&, GameObject& obj) override;
};