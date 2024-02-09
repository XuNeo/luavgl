#pragma once

#include <lauxlib.h>
#include <lua.h>
#include <lvgl/lvgl.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* clang-format off */
#define debug(format, ...)
// fprintf(stderr, "[luavgl] %s: " format, __FUNCTION__, ##__VA_ARGS__)
// syslog(LOG_DEBUG, "[luavgl] %s: " format, __FUNCTION__, ##__VA_ARGS__)
/* clang-format on */

struct event_callback_s {
  lua_State *L;
  lv_event_code_t code;
  int ref; /* ref to callback */
};         /* variable array if n_callback > 1 */

static void dumpstack(lua_State *L);
static void dumptable(lua_State *L, int index);

/* metatable */
int luavgl_obj_getmetatable(lua_State *L, const lv_obj_class_t *clz);
int luavgl_obj_setmetatable(lua_State *L, int idx, const lv_obj_class_t *clz);

static void luavgl_obj_event_init(luavgl_obj_t *lobj);
static void luavgl_obj_remove_event_all(lua_State *L, luavgl_obj_t *obj);

/* util functions */
static void luavgl_check_callable(lua_State *L, int index);
static int luavgl_check_continuation(lua_State *L, int index);

static int luavgl_obj_set_style_kv(lua_State *L, lv_obj_t *obj, int selector);

static int luavgl_pcall_int(lua_State *L, int nargs, int nresult);
