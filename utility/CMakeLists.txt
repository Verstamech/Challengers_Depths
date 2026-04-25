project(utility)

add_library(utility
        vec.cpp vec.h
        physics.cpp physics.h
        toggle.cpp toggle.h
        random.cpp random.h
)

target_link_libraries(utility PUBLIC SDL3::SDL3)
target_include_directories(utility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})