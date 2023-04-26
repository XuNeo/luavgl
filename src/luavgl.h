#pragma once

#include <lua.h>
#if (LUA_VERSION_NUM < 503)
#include "compat-5.3.h"
#endif

#include <lauxlib.h>
#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef const lv_font_t *(*make_font_cb)(const char *name, int size,
                                         int weight);
typedef void (*delete_font_cb)(const lv_font_t *);
typedef int (*luavgl_pcall_t)(lua_State *L, int nargs, int nresults);

typedef struct {
  lv_obj_t *root;
  make_font_cb make_font;
  delete_font_cb delete_font;
  luavgl_pcall_t pcall;
} luavgl_ctx_t;

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

typedef struct {
  const char *key;
  setter_type_t type;

  union {
    void (*setter)(void *, int);
    void (*setter_stack)(void *, lua_State *L);
    void (*setter_pointer)(void *, void *);
  };
} luavgl_value_setter_t;

typedef struct luavgl_obj_s {
  lv_obj_t *obj;    /* NULL means obj deleted, but not gc'ed in lua */
  bool lua_created; /* this object is created from lua */

  /* internally used variables */
  int n_events;
  struct event_callback_s *events;
} luavgl_obj_t;

#define luavgl_obj_newmetatable(L, clz, name, l)                               \
  luavgl_obj_createmetatable(L, clz, name, l, sizeof(l) / sizeof((l)[0]) - 1);

#define luavgl_set_property(L, obj, table)                                     \
  luavgl_set_property_array(L, obj, table, sizeof(table) / sizeof(table[0]))

LUALIB_API luavgl_ctx_t *luavgl_context(lua_State *L);

/**
 * @brief set the protected call used in event callback.
 * this must be called before luaopen_lvgl
 */
LUALIB_API void luavgl_set_pcall(lua_State *L, luavgl_pcall_t pcall);

/**
 * @brief Set luavgl root object
 */
LUALIB_API void luavgl_set_root(lua_State *L, lv_obj_t *root);

/**
 * @brief Set functions to make and destroy extended fonts.
 */
LUALIB_API void luavgl_set_font_extension(lua_State *L, make_font_cb make,
                                          delete_font_cb d);

/* on embedded device, may call lib open manually. */
LUALIB_API int luaopen_lvgl(lua_State *L);

/* object creation */

/**
 * @brief Helper function to create lua lvgl object.
 *
 * Expected stack layout:
 *    [1]: parent object
 *    [2]: table of properties(styles)
 * Return value in stack:
 *    [1]: userdata of created object, luavgl object.
 *
 * @param L
 * @param create function pointer to create object
 * @return items in stack for return
 */
LUALIB_API int luavgl_obj_create_helper(lua_State *L,
                                        lv_obj_t *(*create)(lv_obj_t *parent));

/**
 * @brief Add existing lvgl object to lua
 *
 * Return value in stack:
 *    [1]: luavgl object.
 *
 * @return pointer to luavgl object
 */
LUALIB_API luavgl_obj_t *luavgl_add_lobj(lua_State *L, lv_obj_t *obj);

/**
 * @brief Get lua lvgl object on stack
 *
 * Return value in stack:
 *    [1]: luavgl object.
 *
 * @return pointer to luavgl object
 */
LUALIB_API luavgl_obj_t *luavgl_to_lobj(lua_State *L, int idx);

/**
 * @brief Create metatable for specified object class
 *
 * @param L
 * @param clz object class
 * @param name optional name for this metatable
 * @param l methods for this metatable
 * @param n methods length
 *
 * Return value in stack:
 *    [1]: metatable
 *
 * @return 1
 */
LUALIB_API int luavgl_obj_createmetatable(lua_State *L,
                                          const lv_obj_class_t *clz,
                                          const char *name, const luaL_Reg *l,
                                          int n);

/**
 * @brief Get user value of userdata of lua lvgl object, create if not exists
 *
 * @return type of the uservalue, LUA_TTABLE
 */
LUALIB_API int luavgl_obj_getuserdatauv(lua_State *L, int idx);


/* helper to get value from stack */

/**
 * @brief Get lvgl object from stack
 */
LUALIB_API lv_obj_t *luavgl_to_obj(lua_State *L, int idx);

/**
 * @brief Convert value to integer
 *
 * Supported values are: integer, float, boolean
 */
LUALIB_API int luavgl_tointeger(lua_State *L, int idx);

/**
 * @brief Convert value to lvgl color
 *
 * Supported color formats are:
 *    [1]: string representation: "#RGB" or "#RRGGBB"
 *    [2]: integer value: 0xaabbcc
 */
LUALIB_API lv_color_t luavgl_tocolor(lua_State *L, int idx);

/**
 * @brief Convert value to lvgl image source
 *
 * Supported image format:
 *    [1]: image path string, e.g. "/resource/bg.png"
 *    [2]: image source lightuserdata
 */
LUALIB_API const char *luavgl_toimgsrc(lua_State *L, int idx);

/**
 * @brief Helper function to iterate through table
 *
 * @param L
 * @param index index to the table for iteration
 * @param cb callback function for every key,value pair
 *        Stack layout when cb function made:
 *            [-1]: value
 *            [-2]: key
 *        Care must be taken when calling lua_tostring etc. functions which will
 *        change stack layout.
 * @param cb_param callback parameter passed to cb function
 */
LUALIB_API void luavgl_iterate(lua_State *L, int index,
                               int (*cb)(lua_State *, void *), void *cb_para);

/**
 * @brief Set object property with provided setter functions
 *
 * @param L
 * @param obj lvgl object
 * @param table function array
 * @param len functions in array
 * @return 1
 */
LUALIB_API int luavgl_set_property_array(lua_State *L, void *obj,
                                         const luavgl_value_setter_t table[],
                                         uint32_t len);

/**
 * @brief Dummy property set function
 */
LUALIB_API void _lv_dummy_set(void *obj, lua_State *L);

/**
 * @brief Set object property with key,value in stack
 */
LUALIB_API int luavgl_obj_set_property_kv(lua_State *L, void *data);

/**
 * @brief Set image property with key,value in stack
 */
LUALIB_API int luavgl_img_set_property_kv(lua_State *L, void *data);

LUALIB_API int luavgl_calendar_set_property_kv(lua_State *L, void *data);

LUALIB_API int luavgl_checkbox_set_property_kv(lua_State *L, void *data);

LUALIB_API int luavgl_dropdown_set_property_kv(lua_State *L, void *data);

LUALIB_API int luavgl_keyboard_set_property_kv(lua_State *L, void *data);

LUALIB_API int luavgl_label_set_property_kv(lua_State *L, void *data);

LUALIB_API int luavgl_led_set_property_kv(lua_State *L, void *data);

LUALIB_API int luavgl_list_set_property_kv(lua_State *L, void *data);

LUALIB_API int luavgl_roller_set_property_kv(lua_State *L, void *data);

LUALIB_API int luavgl_textarea_set_property_kv(lua_State *L, void *data);



/**
 * @brief Protected call
 * @return negative error code or zero.
 */
LUALIB_API int luavgl_pcall(lua_State *L, int nargs, int nresult);

/**
 * @brief Check if lua object is callable.
 * @return true if callable, false otherwise.
 */
LUALIB_API int luavgl_is_callable(lua_State *L, int index);

#ifdef __cplusplus
} /*extern "C"*/
#endif
