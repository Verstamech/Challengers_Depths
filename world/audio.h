#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <unordered_map>

struct MIX_Mixer;
struct MIX_Audio;
struct MIX_Track;

struct Sound {
    std::string name;
    std::string filename;
    bool loop_forever{false};
};

class Audio {
public:
    Audio();
    ~Audio();

    void load_sounds(const std::vector<Sound>& sounds_to_load);
    void play_sounds(const std::string& sound_name, bool loop_forever_in_background = false);

private:
    SDL_AudioDeviceID device;
    MIX_Mixer* mixer;
    MIX_Track* background_music;
    std::unordered_map<std::string, MIX_Audio*> sounds;
};