cmake_minimum_required(VERSION 3.5)
project(luavgl LANGUAGES C VERSION 0.0.1)
option(BUILD_SIMULATOR "Build lvgl simulator with luavgl support" ON)
option(FIND_LUA_PACKAGE "Find lua package" ON) # may manually set LUA_INCLUDE_DIR
option(USE_LUAJIT "Use luajit instead of lua" OFF)
option(LV_USE_LIBPNG "Use libpng to decode PNG" OFF)
option(LV_USE_LIBJPEG_TURBO "Use libjpeg turbo to decode JPEG" OFF)
option(LV_USE_FFMPEG "Use libffmpeg to display video using lv_ffmpeg" OFF)
option(LV_USE_FREETYPE "Use freetype lib" OFF)
option(EXPOSE_WIDGET_API "Expose widget create method to global" ON)

add_compile_definitions($<$<BOOL:${LV_USE_DRAW_SDL}>:LV_USE_DRAW_SDL=1>)
add_compile_definitions($<$<BOOL:${LV_USE_LIBPNG}>:LV_USE_LIBPNG=1>)
add_compile_definitions($<$<BOOL:${LV_USE_LIBJPEG_TURBO}>:LV_USE_LIBJPEG_TURBO=1>)
add_compile_definitions($<$<BOOL:${LV_USE_FFMPEG}>:LV_USE_FFMPEG=1>)
add_compile_definitions($<$<BOOL:${EXPOSE_WIDGET_API}>:LUAVGL_EXPOSE_WIDGET_API=1>)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    message(STATUS "ccache found, using it")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif()

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
    target_link_libraries(luavgl lvgl)

    if(LV_USE_LIBPNG)
        find_package(PNG REQUIRED)
        target_include_directories(lvgl PUBLIC ${PNG_INCLUDE_DIR})
        target_link_libraries(luavgl ${PNG_LIBRARY})
    endif(LV_USE_LIBPNG)

    if(LV_USE_LIBJPEG_TURBO)
        # Need to install libjpeg-turbo8-dev
        # `sudo apt install libjpeg-turbo8-dev`
        # `brew install libjpeg-turbo`
        find_package(JPEG REQUIRED)
        target_include_directories(lvgl PUBLIC ${JPEG_INCLUDE_DIRS})
        target_link_libraries(luavgl ${JPEG_LIBRARIES})
    endif(LV_USE_LIBJPEG_TURBO)

    if(LV_USE_FFMPEG)
        target_link_libraries(luavgl avformat avcodec avutil swscale)
    endif(LV_USE_FFMPEG)

    if(LV_USE_FREETYPE)
        find_package(Freetype REQUIRED)
        target_link_libraries(luavgl ${FREETYPE_LIBRARIES})
        target_include_directories(lvgl PUBLIC ${FREETYPE_INCLUDE_DIRS})
    endif(LV_USE_FREETYPE)

endif(BUILD_SIMULATOR)
