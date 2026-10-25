cmake_minimum_required(VERSION 3.24)
project(Engine LANGUAGES C)

find_package(SDL3 REQUIRED)

set(SOURCE_FILES 
    ./source/files.c 
    ./source/image.c 
    ./source/memory.c 
    ./source/main.c
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    SDL3::SDL3
)

# Unix-like systems (Linux/macOS) require -lm; Windows (MSVC) includes math functions by default
if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()