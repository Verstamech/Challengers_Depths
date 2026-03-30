#include "audio.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <filesystem>
#include <fstream>

Audio::Audio() {
    // initialize audio
    if (!SDL_Init(SDL_INIT_AUDIO)) {
        std::string msg{"Audio::Audio() SDL_Init() failed: "};
        throw std::runtime_error(msg + SDL_GetError());
    }

    // open audio device
    SDL_AudioSpec spec{.format = SDL_AUDIO_F32, .channels = 2, .freq = 48000};
    device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (device == 0) {
        SDL_Quit();
        throw std::runtime_error(SDL_GetError());
    }
    if (!MIX_Init()) {
        SDL_CloseAudioDevice(device);
        SDL_Quit();
        throw std::runtime_error(SDL_GetError());
    }
    mixer = MIX_CreateMixerDevice(device, &spec);
    background_music = MIX_CreateTrack(mixer);
}

Audio::~Audio() {
    MIX_DestroyMixer(mixer);
    SDL_CloseAudioDevice(device);
    MIX_Quit();
    SDL_Quit();
}

void Audio::load_sounds(const std::unordered_map<std::string, std::string> sound_files) {
    auto path = std::filesystem::current_path() / "assets" / "Nighttime_Stroll.ogg";
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Could not open: " + path.string());
    }
    MIX_Audio* effect = MIX_LoadAudio(mixer, path.string().c_str(), true);
    if (!effect) {
        std::string msg{SDL_GetError()};
        throw std::runtime_error(msg + "\nUnable to load sound from: " + path.string());
    }
    sounds["background"] = effect;
}

void Audio::play_sounds(const std::string& sound_name, bool loop_forever_in_background) {
    auto sound = sounds.find(sound_name);
    if (sound == sounds.end()) {
        throw std::runtime_error("Cannont find sound: " + sound_name);
    }

    if (loop_forever_in_background) {
        SDL_PropertiesID props = SDL_CreateProperties();
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
        MIX_SetTrackAudio(background_music, sound->second);
        if (!MIX_PlayTrack(background_music, props)) {
            std::string msg{SDL_GetError()};
            throw std::runtime_error(msg + "\nUnable to play sound from: " + sound_name);
        }
    }
    else {
        if (!MIX_PlayAudio(mixer, sound->second)) {
            std::string msg{SDL_GetError()};
            throw std::runtime_error(msg + "\nUnable to play sound from: " + sound_name);
        }
    }
}
