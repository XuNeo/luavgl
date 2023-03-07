cmake_minimum_required(VERSION 3.1)
project(lugl LANGUAGES C VERSION 0.0.1)
option(BUILD_SIMULATOR "Build lvgl simulator with lugl support" OFF)
option(FIND_LUA_PACKAGE "Find lua package" ON) # may manually set LUA_INCLUDE_DIR

add_library(lugl STATIC "src/lugl.c")

if (FIND_LUA_PACKAGE)
    find_package(Lua)
endif(FIND_LUA_PACKAGE)

if (BUILD_SIMULATOR)
    add_compile_options(-g -O0)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)

    find_package(SDL2 REQUIRED SDL2)
    include_directories(${SDL2_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/simulator) # need lv_conf.h and lv_drv_conf.h
    add_compile_definitions(LUGL_EXAMPLE_DIR="${CMAKE_SOURCE_DIR}/examples")

    add_subdirectory(deps/lvgl)
    add_subdirectory(deps/lv_drivers)
    add_subdirectory(simulator)
    target_include_directories(lugl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/lvgl) # need lvgl.h
    target_include_directories(lugl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/simulator) # need lv_conf.h
endif(BUILD_SIMULATOR)

target_include_directories(lugl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(lugl PRIVATE ${LUA_INCLUDE_DIR}) # need lua.h, lauxlib.h
target_compile_options(lugl PRIVATE -Wall -Werror)
