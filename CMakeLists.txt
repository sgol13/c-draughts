cmake_minimum_required(VERSION 3.10.2)

project(c-draughts)

add_definitions(-DRESOURCES_DIR="../resources/")

find_package(LibXml2 REQUIRED)

add_executable(game source/main.c source/game.c source/interface.c)
target_include_directories(game PRIVATE include ${LIBXML2_INCLUDE_DIR})

target_link_libraries(game ${LIBXML2_LIBRARIES})
