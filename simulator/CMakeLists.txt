cmake_minimum_required(VERSION 3.10)

project(simulator)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

find_package(SDL2 REQUIRED SDL2)
include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}/../
        ${PROJECT_SOURCE_DIR}
)

SET(CMAKE_CXX_FLAGS "-O0 -g")
set(CMAKE_C_FLAGS "-g")

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_definitions( -DVERSION_LIBINTERFACE=${LIBINTERFACE_VERSION} )

add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main)

add_executable(main main.c mouse_cursor_icon.c)
target_compile_definitions(main PRIVATE LUGL_EXAMPLE_DIR="${CMAKE_SOURCE_DIR}/examples")
target_link_libraries(main lugl lvgl lvgl::demos lvgl::drivers ${SDL2_LIBRARIES} ${LUA_LIBRARIES})
