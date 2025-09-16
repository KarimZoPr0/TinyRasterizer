cmake_minimum_required(VERSION 3.10)
project(TinyRasterizer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")

add_library(game SHARED code/game/game.c)

add_executable(TinyRasterizer code/main.c)
add_executable(AtlasGen code/atlas_gen/atlasGen.c)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(TinyRasterizer PRIVATE $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(AtlasGen PRIVATE $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)

find_package(SDL2_image CONFIG REQUIRED)
target_link_libraries(AtlasGen PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)

target_link_libraries(game PRIVATE $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)

