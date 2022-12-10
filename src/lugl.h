#pragma once

#include <debug.h>
#include <lua.h>
#include <lvgl.h>

typedef const lv_font_t *(*make_font_cb)(const char *name, int size,
                                         int weight);
typedef void (*delete_font_cb)(const lv_font_t *);

typedef struct {
  lv_obj_t *root;
  make_font_cb make_font;
  delete_font_cb delete_font;
} lugl_ctx_t;

LUALIB_API lugl_ctx_t *lugl_context(lua_State *L);

LUALIB_API void lugl_set_root(lua_State *L, lv_obj_t *root);
LUALIB_API void lugl_set_font_extension(lua_State *L, make_font_cb make,
                                        delete_font_cb delete);
