cmake_minimum_required(VERSION 3.15)
project(raycasting-engine C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SRC_FILES
    main.c
    map.c
    window.c
    game_manager.c
    player.c
    controller.c
    transform.c
    ui.c
    frame_rate.c
    raycaster.c
    sprite.c
)

list(TRANSFORM SRC_FILES PREPEND src/)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(flecs CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ./include)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image-static)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_ttf::SDL3_ttf-static)
target_link_libraries(${PROJECT_NAME} PRIVATE flecs::flecs_static)
