#include "graphics.h"
#include <stdexcept>
#include <SDL3/SDL.h>
#include <SDL3/SDL_surface.h>

Graphics::Graphics(const std::string &title, int window_width, int window_height)
    : title{title}, width{window_width}, height{window_height} {

    SDL_SetAppMetadata(title.data(), "1.0", NULL);

    if (!SDL_CreateWindowAndRenderer(title.data(), window_width, window_height, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    }

    SDL_SetRenderLogicalPresentation(renderer, window_width, window_height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void Graphics::draw(const SDL_FRect& rect, const Color& color, bool filled) {
    auto [red, green, blue, alpha] = color;
    SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
    if (filled) {
        SDL_RenderFillRect(renderer, &rect);
    }
    else {
        SDL_RenderRect(renderer, &rect);
    }
}

void Graphics::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderClear(renderer);
}

void Graphics::update() {
    SDL_RenderPresent(renderer);
}

void Graphics::set_title(const std::string &title) {
    SDL_SetWindowTitle(window, title.c_str());
}

int Graphics::get_texture_id(const std::string &image_filename) {
    auto search = texture_ids.find(image_filename);
    if (search != texture_ids.end()) {
        int texture_id = search->second;
        return texture_id;
    }
    else { // this is a new image filename
        // SDL_Texture* texture = IMG_LoadTexture(renderer, image_filename.data());
        SDL_Surface* surface = SDL_LoadPNG(image_filename.c_str());
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
        if (!texture) {
            throw std::runtime_error(SDL_GetError());
        }

        int texture_id = textures.size();
        texture_ids[image_filename] = texture_id;

        // Retain ownership of the texture pointer
        textures.push_back(texture);

        return texture_id;
    }
}

void Graphics::draw_sprite(const Vec<float> &pixel, const Sprite &sprite) {
    if (sprite.texture_id < 0) {
        return;
    }

    float x = pixel.x + sprite.shift.x * sprite.scale;
    float y = pixel.y + sprite.shift.y * sprite.scale;
    float w = sprite.size.x * sprite.scale;
    float h = sprite.size.y * sprite.scale;

    SDL_FRect screen_pixels{x, y, w, h};
    SDL_FPoint center{sprite.center.x * sprite.scale, sprite.center.y * sprite.scale};

    SDL_FRect image_pixels{sprite.location.x, sprite.location.y, sprite.size.x, sprite.size.y};
    SDL_Texture* texture = textures.at(sprite.texture_id);
    SDL_FlipMode flip = sprite.flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    SDL_RenderTextureRotated(renderer, texture, &image_pixels, &screen_pixels, sprite.angle, &center, flip);
}
