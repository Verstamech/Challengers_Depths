#pragma once
#include <map>
#include <string>

class Event;
class World;
class GameObject;

using Events = std::map<std::string, Event*>;

class Event {
public:
    virtual ~Event() = default;
    virtual void perform(World& world, GameObject& obj) = 0;
};

class NextLevel : public Event {
public:
    void perform(World& world, GameObject& obj) override;
};