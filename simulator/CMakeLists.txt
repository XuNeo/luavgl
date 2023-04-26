cmake_minimum_required(VERSION 3.10)

project(simulator)

add_custom_target(run COMMAND simulator)

add_executable(simulator main.c mouse_cursor_icon.c extension.c lv_pointer.c pointer.c)
target_link_libraries(simulator luavgl lvgl lvgl::demos lvgl::drivers ${SDL2_LIBRARIES} ${LUA_LINK_LIBS})
target_include_directories(simulator PRIVATE ${LUA_INCLUDE_DIRS})
