cmake_minimum_required(VERSION 3.10)

project(simulator)

add_custom_target(run COMMAND simulator)

file(GLOB_RECURSE WIDGETS ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.c)
add_executable(simulator main.c mouse_cursor_icon.c ${WIDGETS})
target_include_directories(simulator PRIVATE widgets)
# add_subdirectory(widgets)

target_link_libraries(simulator luavgl lvgl lvgl::demos lvgl::drivers ${SDL2_LIBRARIES} ${LUA_LINK_LIBS})
target_include_directories(simulator PRIVATE ${LUA_INCLUDE_DIRS})
