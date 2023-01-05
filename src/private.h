#pragma once

#include <lauxlib.h>
#include <lua.h>
#include <lvgl.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* clang-format off */
#define debug(format, ...)
// fprintf(stdout, "[lugl] %s: " format, __FUNCTION__, ##__VA_ARGS__)
// syslog(LOG_DEBUG, "[lugl] %s: " format, __FUNCTION__, ##__VA_ARGS__)
/* clang-format on */

struct event_callback_s {
  lv_event_code_t code;
  struct _lv_event_dsc_t *dsc;
  int ref; /* ref to callback */
};         /* variable array if n_callback > 1 */

static void dumpstack(lua_State *L);
static void dumptable(lua_State *L, int index);

static lugl_obj_t *lugl_to_lobj(lua_State *L, int idx);
static lugl_obj_t *lugl_obj_touserdatauv(lua_State *L, int idx);

/* metatable */
int lugl_obj_getmetatable(lua_State *L, const lv_obj_class_t *clz);
int lugl_obj_setmetatable(lua_State *L, int idx, const lv_obj_class_t *clz);

static void lugl_obj_event_init(lugl_obj_t *lobj);
static void lugl_obj_anim_init(lugl_obj_t *lobj);
static void lugl_obj_remove_event_all(lua_State *L, lugl_obj_t *obj);
static int lugl_obj_remove_all_anim_int(lua_State *L, lugl_obj_t *lobj);

/* util functions */
static int lugl_is_callable(lua_State *L, int index);
static void lugl_check_callable(lua_State *L, int index);
static int lugl_check_continuation(lua_State *L, int index);

static int lugl_obj_set_style_kv(lua_State *L, lv_obj_t *obj, int selector);
