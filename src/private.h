#pragma once

#include <lauxlib.h>
#include <lua.h>
#include <lvgl.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* clang-format off */
#define debug(format, ...) fprintf(stdout, "[lugl] %s: " format, __FUNCTION__, ##__VA_ARGS__)
// syslog(LOG_DEBUG, "[lugl] %s: " format, __FUNCTION__, ##__VA_ARGS__)
/* clang-format on */

typedef enum {
  /* type can be regarded as int */
  SETTER_TYPE_INT = 0,
  SETTER_TYPE_COLOR,

  /* type of data from stack */
  SETTER_TYPE_STACK,   /* parameter is on stack -1 */
  SETTER_TYPE_POINTER, /* parameter from lightuserdata, a pointer */
  SETTER_TYPE_IMGSRC,
} setter_type_t;

typedef void (*setter_int_t)(void *, int);

/* clang-format off */
typedef struct {
    const char* key;
    setter_type_t type;
    union {
        void (*setter)(void*, int);
        void (*setter_stack)(void*, lua_State* L);
        void (*setter_pointer)(void*, void*);
    };
} lugl_value_setter_t;
/* clang-format on */

typedef struct lugl_anim_s {
  lv_anim_t *aa; /* the handler returns be lv_anim_start */
  lv_anim_t cfg; /* the configuration, must not be the first element. */
  lv_obj_t *obj;
  lua_State *L;
  int ref; /* execute callback to reference. */

  /* anim configured but not started. lv_anim does't support pause anim, this
   * is done by keep a lv_anim_t backup, thus we can re-start the anim any
   * time.
   *
   * deleted = ref == LUA_NOREF
   */
  bool paused;
} lugl_anim_t;

typedef lugl_anim_t *lugl_anim_handle_t;

struct event_callback_s {
  lv_event_code_t code;
  struct _lv_event_dsc_t *dsc;
  int ref; /* ref to callback */
};         /* variable array if n_callback > 1 */

/* Ref for userdata and event callbacks */
typedef struct {
  lv_obj_t *obj;
  bool lua_created; /* this object is created from lua */

  int n_anim;
  int n_events;
  lugl_anim_handle_t *anims; /* store handler to anim */
  struct event_callback_s *events;
} lugl_obj_t;

static void dumpstack(lua_State *L);
static const char *lugl_class_to_metatable_name(lv_obj_t *obj);

static lugl_obj_t *lugl_new_obj(lua_State *L, lv_obj_t *obj);
static lugl_obj_t *lugl_to_lobj(lua_State *L, int idx);
static lv_obj_t *lugl_check_obj(lua_State *L, int index);

static int lugl_obj_create_helper(lua_State *L,
                                  lv_obj_t *(*create)(lv_obj_t *parent));

static void lugl_obj_event_init(lugl_obj_t *lobj);
static void lugl_obj_anim_init(lugl_obj_t *lobj);
static void lugl_obj_remove_event_all(lua_State *L, lugl_obj_t *obj);
static int lugl_obj_remove_all_anim_int(lua_State *L, lugl_obj_t *lobj);

/* util functions */
static int lugl_is_callable(lua_State *L, int index);
static void lugl_check_callable(lua_State *L, int index);
static int lugl_check_continuation(lua_State *L, int index);

static void lugl_iterate(lua_State *L, int index,
                         int (*cb)(lua_State *, void *), void *cb_para);

static int _lugl_set_property(lua_State *L, void *obj,
                              const lugl_value_setter_t table[], uint32_t len);

#define lugl_set_property(L, obj, table)                                       \
  _lugl_set_property(L, obj, table, sizeof(table) / sizeof(table[0]))

static int lugl_obj_set_property_kv(lua_State *L, void *data);

static const char *lugl_toimgsrc(lua_State *L, int idx);

static void _lv_dummy_set(void *obj, lua_State *L);

static int lugl_tointeger(lua_State *L, int idx);
static int lugl_obj_set_style_kv(lua_State *L, lv_obj_t *obj, int selector);
static lv_color_t lugl_tocolor(lua_State *L, int idx);

static void lugl_new_objlib(lua_State *L);
