cmake_minimum_required(VERSION 3.15)
project(raycasting-engine C)

find_package(SDL3 REQUIRED)

add_executable(${PROJECT_NAME} src/main.c)
target_link_libraries(${PROJECT_NAME} SDL3::SDL3)