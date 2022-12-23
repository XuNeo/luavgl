cmake_minimum_required(VERSION 3.10)

project(simulator)

add_custom_target(run COMMAND main)

add_executable(main main.c mouse_cursor_icon.c)
target_link_libraries(main lugl lvgl lvgl::demos lvgl::drivers ${SDL2_LIBRARIES} ${LUA_LIBRARIES})
target_include_directories(main PRIVATE ${LUA_INCLUDE_DIR})
