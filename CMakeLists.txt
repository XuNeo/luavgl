cmake_minimum_required(VERSION 3.1)
project(lugl LANGUAGES C VERSION 0.0.1)

find_package(Lua)

set(CMAKE_C_FLAGS "-g")

MESSAGE(STATUS "LUA_INCLUDE_DIR: " ${LUA_INCLUDE_DIR})
MESSAGE(STATUS "LVGL_INCLUDE_DIR: " ${LVGL_INCLUDE_DIR})

set(LUA_LIB_SRCS "src/lugl.c")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/simulator)

include_directories(${LUA_INCLUDE_DIR})

include_directories(deps/lvgl)
add_subdirectory(deps/lvgl)

add_library(lugl STATIC ${LUA_LIB_SRCS})
target_link_libraries(lugl lvgl)
target_include_directories(lugl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# add_library(lugl MODULE ${LUA_LIB_SRCS})
# set_target_properties(lugl PROPERTIES PREFIX "")

add_subdirectory(deps/lv_drivers)
add_subdirectory(simulator)
