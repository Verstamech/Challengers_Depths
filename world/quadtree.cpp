#include "quadtree.h"
#include "game_object.h"

bool AABB::contains(Vec<float> point) const {
    Vec<float> displacement = point - center;
    return (std::abs(displacement.x) < half_dimension.x) && (std::abs(displacement.y) < half_dimension.y);
}

bool AABB::intersects(AABB other) const {
    Vec<float> displacement = other.center - center;
    return (std::abs(displacement.x) < (half_dimension.x + other.half_dimension.x)) && (std::abs(displacement.y) < (half_dimension.y + other.half_dimension.y));
}

QuadTree::QuadTree(AABB boundary)
    : boundary{boundary} {}

void QuadTree::clear() {
    objects.clear();
    north_west = nullptr;
    north_east = nullptr;
    south_west = nullptr;
    south_east = nullptr;
}

bool QuadTree::insert(GameObject *obj) {
    // Ignore objects not in this node
    if (!boundary.contains(obj->get_bounding_box().center)) {
        return false;
    }

    // If there is space, add it
    if (objects.size() < NODE_CAPACITY && north_west == nullptr) {
        objects.push_back(obj);
        return true;
    }

    // Otherwise, subdivide and insert into children
    if (north_west == nullptr) {
        subdivide();
    }

    return north_west->insert(obj) || north_east->insert(obj) || south_west->insert(obj) || south_east->insert(obj);
}

std::vector<GameObject *> QuadTree::query_range(AABB range) const {
    if (!boundary.intersects(range)) {
        return {};
    }

    // handle leaf node
    std::vector<GameObject*> results;
    if (north_west == nullptr) {
        std::copy_if(std::begin(objects), std::end(objects), std::back_inserter(results),
                [&](GameObject* obj) {return range.intersects(obj->get_bounding_box());}
        );
        return results;
    }

    // add objects from children
    auto pts = north_west->query_range(range);
    results.insert(std::end(results), std::begin(pts), std::end(pts));
    pts = north_east->query_range(range);
    results.insert(std::end(results), std::begin(pts), std::end(pts));
    pts = south_west->query_range(range);
    results.insert(std::end(results), std::begin(pts), std::end(pts));
    pts = south_east->query_range(range);
    results.insert(std::end(results), std::begin(pts), std::end(pts));

    return results;
}

void QuadTree::subdivide() {
    Vec<float> half = boundary.half_dimension / 2.0f;

    north_west = std::make_shared<QuadTree>(AABB{{boundary.center.x - half.x, boundary.center.y + half.y}, half});
    north_east = std::make_shared<QuadTree>(AABB{{boundary.center.x + half.x, boundary.center.y + half.y}, half});
    south_west = std::make_shared<QuadTree>(AABB{{boundary.center.x - half.x, boundary.center.y - half.y}, half});
    south_east = std::make_shared<QuadTree>(AABB{{boundary.center.x + half.x, boundary.center.y - half.y}, half});

    for (auto obj : objects) {
        insert(obj);
    }
    objects.clear();
}