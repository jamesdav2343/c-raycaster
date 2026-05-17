cmake_minimum_required(VERSION 3.15)
project(raycasting-engine C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SRC_FILES
    main.c
    map.c
    window.c
    game_manager.c
    player.c
    ui.c
    sprite.c
    camera.c
    math_utils.c
    general_utils.c
    ht.c
    debug.c
    components/input.c
    components/transform.c
    components/raycaster.c
    systems/input.c
    systems/transform.c
    systems/raycaster.c
    lighting.c
)

list(TRANSFORM SRC_FILES PREPEND src/)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(flecs CONFIG REQUIRED)
find_package(glib REQUIRED)
find_package(cJSON REQUIRED)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ./include)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image-static)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_ttf::SDL3_ttf-static)
target_link_libraries(${PROJECT_NAME} PRIVATE flecs::flecs_static)
target_link_libraries(${PROJECT_NAME} PRIVATE glib::glib)
target_link_libraries(${PROJECT_NAME} PRIVATE cjson::cjson)
