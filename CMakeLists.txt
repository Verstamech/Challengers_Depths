cmake_minimum_required(VERSION 3.24)

project(challengers_depths)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-g -Wall -Wextra -pedantic)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS ON)

# On Windows, create dll's in the same directory as the game executable
if (WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
        SDL
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
        SDL_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG main
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
)
# disable formats we don't use to make the build faster and smaller
set(SDLMIXER_MIDI_NATIVE OFF)
set(SDLMIXER_GME OFF)
set(SDLMIXER_WAVPACK OFF)
set(SDLMIXER_MOD OFF)
set(SDLMIXER_OPUS OFF)

# FetchContent_MakeAvailable(SDL SDL_mixer SDL_image)
FetchContent_MakeAvailable(SDL SDL_mixer)

add_subdirectory(utility)
add_subdirectory(graphics)
add_subdirectory(world)

add_library(game_lib
        game.cpp game.h
        asset_manager.cpp asset_manager.h
        level_designer.cpp level_designer.h
)

target_link_libraries(game_lib PUBLIC SDL3::SDL3 SDL3_mixer::SDL3_mixer graphics utility world)

add_executable(challengers_depths main.cpp)
target_link_libraries(challengers_depths PUBLIC game_lib)

add_executable(level_designer main_designer.cpp)
target_link_libraries(level_designer PUBLIC game_lib)