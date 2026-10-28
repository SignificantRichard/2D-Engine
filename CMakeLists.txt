cmake_minimum_required(VERSION 3.24)
project(Engine LANGUAGES C)

find_package(SDL3 REQUIRED)

set(SOURCE_FILES 
    ./source/engine/vector.c
    ./source/engine/body.c
    ./source/engine/edge.c
    ./source/engine/face.c
    ./source/engine/files.c 
    ./source/engine/image.c 
    ./source/engine/memory.c
    ./source/engine/sound.c
    ./source/engine/raycast.c
    ./source/engine/renderqueue.c

    ./source/game/game_main.c
    
    ./source/engine/main.c
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    SDL3::SDL3
)

# Unix-like systems (Linux/macOS) require -lm; Windows (MSVC) includes math functions by default
if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()