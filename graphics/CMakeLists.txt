project(graphics)

add_library(graphics
    graphics.cpp graphics.h
    sprite.h
)

target_include_directories(graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(graphics PUBLIC SDL3::SDL3)