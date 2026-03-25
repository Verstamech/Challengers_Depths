#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL.h>
#include "sprite.h"
#include "vec.h"

class Color {
public:
    int red{255}, green{0}, blue{0}, alpha{255};
};

class Graphics {
public:
    Graphics(const std::string& title, int window_width, int window_height);
    void clear();
    void update();
    void set_title(const std::string& title);

    void draw(const SDL_FRect&, const Color& color, bool filled=true);
    void draw_sprite(const Vec<float>& pixel, const Sprite& sprite);

    int get_texture_id(const std::string& image_filename);

    const int width;
    const int height;

private:
    std::string title;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<SDL_Texture*> textures;
    std::unordered_map<std::string, int> texture_ids;
};