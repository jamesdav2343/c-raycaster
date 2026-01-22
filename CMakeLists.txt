cmake_minimum_required(VERSION 3.15)
project(raycasting-engine C)
include_directories(./include)

set(SRC_FILES
    main.c
    map.c
    window.c
    game_manager.c
)

list(TRANSFORM SRC_FILES PREPEND src/)

find_package(SDL3 REQUIRED)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} SDL3::SDL3)
