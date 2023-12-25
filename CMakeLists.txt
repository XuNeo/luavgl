cmake_minimum_required(VERSION 3.1)
project(luavgl LANGUAGES C VERSION 0.0.1)
option(BUILD_SIMULATOR "Build lvgl simulator with luavgl support" ON)
option(FIND_LUA_PACKAGE "Find lua package" ON) # may manually set LUA_INCLUDE_DIR
option(USE_LUAJIT "Use luajit instead of lua" OFF)


if (BUILD_SIMULATOR)
    add_compile_options(-g -O0)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif(BUILD_SIMULATOR)

add_library(luavgl STATIC "src/luavgl.c")

# Search in the 'cmake' directory for additional CMake modules.
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (FIND_LUA_PACKAGE)
    if (USE_LUAJIT)
        find_package(LuaJIT REQUIRED)
        list(APPEND LUA_INCLUDE_DIRS ${LUAJIT_INCLUDE_DIR})
		list(APPEND LUA_LINK_LIBS ${LUAJIT_LIBRARY})
    else()
        find_package(Lua)
        list(APPEND LUA_INCLUDE_DIRS ${LUA_INCLUDE_DIR})
		list(APPEND LUA_LINK_LIBS ${LUA_LIBRARIES})
    endif(USE_LUAJIT)
endif(FIND_LUA_PACKAGE)

target_include_directories(luavgl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(luavgl PRIVATE ${LUA_INCLUDE_DIRS}) # need lua.h, lauxlib.h
target_link_libraries(luavgl ${LUA_LINK_LIBS})
target_compile_options(luavgl PRIVATE -Wall -Werror)

if (BUILD_SIMULATOR)
    find_package(SDL2 REQUIRED SDL2)
    include_directories(${SDL2_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/simulator) # need lv_conf.h and lv_drv_conf.h
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/lua-compat-5.3/c-api)
    add_compile_definitions(LUAVGL_EXAMPLE_DIR="${CMAKE_SOURCE_DIR}/examples")

    add_subdirectory(deps/lvgl)
    add_subdirectory(simulator)
    target_include_directories(luavgl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps) # need lvgl/lvgl.h
    target_include_directories(simulator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps) # need lvgl/lvgl.h
endif(BUILD_SIMULATOR)
