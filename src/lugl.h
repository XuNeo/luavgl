#pragma once

#include <lauxlib.h>
#include <lua.h>
#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef const lv_font_t *(*make_font_cb)(const char *name, int size,
                                         int weight);
typedef void (*delete_font_cb)(const lv_font_t *);

typedef struct {
  lv_obj_t *root;
  make_font_cb make_font;
  delete_font_cb delete_font;
} lugl_ctx_t;

typedef enum {
  /* type can be regarded as int */
  SETTER_TYPE_INT = 0,
  SETTER_TYPE_COLOR,

  SETTER_TYPE_STACK,   /* parameter is on stack -1 */
  SETTER_TYPE_POINTER, /* parameter from lightuserdata, a pointer */
  SETTER_TYPE_IMGSRC,
} setter_type_t;

typedef void (*setter_int_t)(void *, int);
typedef void (*setter_pointer_t)(void *, void *);

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

/* Ref for userdata and event callbacks */
typedef struct lugl_obj_s {
  lv_obj_t *obj;
  bool lua_created; /* this object is created from lua */

  /* internally used variables */
  int n_anim;
  int n_events;
  struct lugl_anim_s **anims; /* store handler to anim */
  struct event_callback_s *events;
} lugl_obj_t;

#define lugl_obj_newmetatable(L, clz, name, l)                                 \
  lugl_obj_createmetatable(L, clz, name, l, sizeof(l) / sizeof((l)[0]) - 1);

#define lugl_set_property(L, obj, table)                                       \
  lugl_set_property_array(L, obj, table, sizeof(table) / sizeof(table[0]))

LUALIB_API lugl_ctx_t *lugl_context(lua_State *L);

LUALIB_API void lugl_set_root(lua_State *L, lv_obj_t *root);
LUALIB_API void lugl_set_font_extension(lua_State *L, make_font_cb make,
                                        delete_font_cb d);

/* on embedded device, may call lib open manually. */
int luaopen_lugl(lua_State *L);

/* object creation */
int lugl_obj_create_helper(lua_State *L, lv_obj_t *(*create)(lv_obj_t *parent));
lugl_obj_t *lugl_add_lobj(lua_State *L, lv_obj_t *obj);

/* object metatable */
int lugl_obj_createmetatable(lua_State *L, const lv_obj_class_t *clz,
                             const char *name, const luaL_Reg *l, int n);

/* helper to get value from stack */
lv_obj_t *lugl_to_obj(lua_State *L, int idx);
int lugl_tointeger(lua_State *L, int idx);
lv_color_t lugl_tocolor(lua_State *L, int idx);
const char *lugl_toimgsrc(lua_State *L, int idx);

void lugl_iterate(lua_State *L, int index, int (*cb)(lua_State *, void *),
                  void *cb_para);
int lugl_set_property_array(lua_State *L, void *obj,
                            const lugl_value_setter_t table[], uint32_t len);
void _lv_dummy_set(void *obj, lua_State *L);

int lugl_obj_set_property_kv(lua_State *L, void *data);

#ifdef __cplusplus
} /*extern "C"*/
#endif
