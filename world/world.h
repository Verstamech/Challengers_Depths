#pragma once
#include <memory>

#include "physics.h"
#include "tilemap.h"
#include "vec.h"
#include "keyboard_input.h"
#include <map>
#include "events.h"
#include "projectile.h"
#include "quadtree.h"

class GameObject;
class Level;
class Audio;
class QuadTree;

class World {
public:
    World(const Level& level, Audio& audio, GameObject* player, Events events);
    ~World();

    void add_platform(float x, float y, float width, float height);
    bool collides(const Vec<float>& position) const;
    void update(float dt);
    void update_object(GameObject* obj, float dt);
    void load_level(const Level& level);

    void move_to(Vec<float>& position, const Vec<int>& size, Vec<float>& velocity);

    Tilemap tilemap;
    bool end_level{false};
    bool end_game{false};
    void build_quadtree();

    GameObject* player;
    Physics physics;
    Audio* audio;
    Events events;
    std::vector<GameObject*> game_objects;
    std::vector<Projectile*> projectiles;

    void touch_tiles(GameObject& obj);

    QuadTree quadtree;

    // items
    std::map<std::string, std::function<GameObject*()>> available_items;
};