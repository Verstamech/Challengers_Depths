project(world)

add_library(world
        world.cpp world.h
        tilemap.cpp tilemap.h
        camera.cpp camera.h
        game_object.cpp game_object.h
        action.cpp action.h
        fsm.cpp fsm.h
        states.cpp states.h
        input.cpp input.h
        keyboard_input.cpp keyboard_input.h
        level.cpp level.h
        audio.cpp audio.h
)

target_include_directories(world PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(world PUBLIC SDL3::SDL3 SDL3_mixer::SDL3_mixer graphics utility)