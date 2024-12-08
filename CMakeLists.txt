cmake_minimum_required(VERSION 3.10)
project(TinyRasterizer C)

set(CMAKE_C_STANDARD 17)

add_library(game SHARED code/game/game.c)
add_executable(TinyRasterizer code/main.c)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(TinyRasterizer
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

target_link_libraries(game
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)