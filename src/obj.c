#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include "lugl.h"
#include "private.h"

struct obj_style_selector {
  lv_obj_t *obj;
  lv_style_selector_t selector;
};

static int obj_set_style_cb(lua_State *L, void *data);

static int lugl_error(lua_State *L, const char *msg)
{
  lua_pushnil(L);
  lua_pushstring(L, msg);
  return 2;
}

static lv_obj_t *lugl_check_obj(lua_State *L, int index)
{
  lv_obj_t *obj;
  void *udata;
  if (!(udata = lua_touserdata(L, index))) {
    goto fail;
  }

  if (!(obj = *(lv_obj_t **)udata)) {
    /* could be already deleted, but not gc'ed */
    return NULL;
  }

  return obj;
fail:
  debug("arg not lv_obj userdata.\n");
  luaL_argerror(L, index, "Expected lv_obj userdata");
  return NULL;
}

static lugl_obj_data_t *lugl_obj_alloc_data(lua_State *L, lv_obj_t *obj)
{
  /**
   * obj has no userdata, add one.
   *
   * This obj should not be gc'ed by lua, since it's not created by
   * lua
   */
  lugl_obj_data_t *data = calloc(sizeof(*data), 1);
  obj->user_data = data;

  lugl_obj_event_init(obj);
  lugl_obj_anim_init(obj);
  data->non_native = true;
  data->obj = obj;
  data->L = L;
  return data;
}

static void _lv_obj_set_align(void *obj, lua_State *L)
{
  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "should be table.");
    debug("para should be table.");
    return;
  }

  lua_getfield(L, -1, "align");
  lv_align_t align = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "x_ofs");
  lv_coord_t x_ofs = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "y_ofs");
  lv_coord_t y_ofs = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lv_obj_align(obj, align, x_ofs, y_ofs);
}

static void _lv_obj_set_align_to(void *obj, lua_State *L)
{
  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "should be table.");
    debug("para should be table.");
    return;
  }

  lua_getfield(L, -1, "align");
  lv_align_t align = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "base");
  lv_obj_t *base = lugl_check_obj(L, -1);
  lua_pop(L, 1);
  if (base == NULL) {
    luaL_argerror(L, -1, "base is not lua obj");
    debug("base obj should be created by lua");
    return;
  }

  lua_getfield(L, -1, "x_ofs");
  lv_coord_t x_ofs = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "y_ofs");
  lv_coord_t y_ofs = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lv_obj_align_to(obj, base, align, x_ofs, y_ofs);
}

static const lugl_value_setter_t obj_property_table[] = {
    {"x", 0, {.setter = (setter_int_t)lv_obj_set_x}},
    {"y", 0, {.setter = (setter_int_t)lv_obj_set_y}},
    {"w", 0, {.setter = (setter_int_t)lv_obj_set_width}},
    {"h", 0, {.setter = (setter_int_t)lv_obj_set_height}},
    {"align", SETTER_TYPE_STACK, {.setter_stack = _lv_obj_set_align}},
    {"align_to", SETTER_TYPE_STACK, {.setter_stack = _lv_obj_set_align_to}},
};

static int obj_set_property_cb(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = lugl_set_property(L, obj, obj_property_table);

  if (ret == 0)
    return 0;

  struct obj_style_selector obj_style = {.obj = obj, .selector = 0};

  return obj_set_style_cb(L, &obj_style);
}

static void _lv_obj_set_style_bg_img_src(void *obj, lua_State *L,
                                         lv_style_selector_t selector)
{
  const char *src = lugl_get_img_src(L, -1);
  if (src == NULL)
    return;

  lv_obj_set_style_bg_img_src(obj, src, selector);
}

static void _lv_obj_set_style_arc_img_src(void *obj, lua_State *L,
                                          lv_style_selector_t selector)
{
  const char *src = lugl_get_img_src(L, -1);
  if (src == NULL)
    return;

  lv_obj_set_style_arc_img_src(obj, src, selector);
}

/* clang-format off */
static const lugl_value_setter_t obj_style_table[] = {
    { "width", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_width } },
    { "min_width", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_width } },
    { "max_width", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_width } },
    { "height", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_width } },
    { "min_height", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_width } },
    { "max_height", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_width } },
    { "x", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_x } },
    { "y", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_y } },
    { "size", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_size } },
    { "align", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_align } },
    { "transform_width", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_transform_width } },
    { "transform_height", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_transform_height } },
    { "translate_x", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_translate_x } },
    { "translate_y", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_translate_y } },
    { "transform_zoom", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_transform_zoom } },
    { "transform_angle", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_transform_angle } },
    { "pad_all", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_pad_all } },
    { "pad_top", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_pad_top } },
    { "pad_bottom", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_pad_bottom } },
    { "pad_ver", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_pad_ver } },
    { "pad_left", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_pad_left } },
    { "pad_right", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_pad_right } },
    { "pad_hor", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_pad_hor } },
    { "pad_row", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_pad_row } },
    { "pad_column", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_pad_column } },
    { "pad_gap", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_pad_gap } },
    { "bg_color", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_bg_color } },
    { "bg_opa", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_bg_opa } },
    { "bg_grad_color", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_bg_grad_color } },
    { "bg_grad_dir", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_bg_grad_dir } },
    { "bg_main_stop", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_bg_main_stop } },
    { "bg_grad_stop", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_bg_grad_stop } },
    // { "bg_grad", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_bg_grad } },
    { "bg_dither_mode", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_bg_dither_mode } },
    { "bg_img_src", SETTER_TYPE_STACK, { .setter_stack_s = _lv_obj_set_style_bg_img_src } },
    { "bg_img_opa", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_bg_img_opa } },
    { "bg_img_recolor", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_bg_img_recolor } },
    { "bg_img_recolor_opa", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_bg_img_recolor_opa } },
    { "bg_img_tiled", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_bg_img_tiled } },
    { "border_color", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_border_color } },
    { "border_opa", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_border_opa } },
    { "border_width", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_border_width } },
    { "border_side", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_border_side } },
    { "border_post", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_border_post } },
    { "outline_width", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_outline_width } },
    { "outline_color", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_outline_color } },
    { "outline_opa", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_outline_opa } },
    { "outline_pad", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_outline_pad } },
    { "shadow_width", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_shadow_width } },
    { "shadow_ofs_x", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_shadow_ofs_x } },
    { "shadow_ofs_y", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_shadow_ofs_y } },
    { "shadow_spread", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_shadow_spread } },
    { "shadow_color", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_shadow_color } },
    { "shadow_opa", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_shadow_opa } },
    { "img_opa", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_img_opa } },
    { "img_recolor", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_img_recolor } },
    { "img_recolor_opa", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_img_recolor_opa } },
    { "line_width", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_line_width } },
    { "line_dash_width", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_line_dash_width } },
    { "line_dash_gap", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_line_dash_gap } },
    { "line_rounded", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_line_rounded } },
    { "line_color", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_line_color } },
    { "line_opa", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_line_opa } },
    { "arc_width", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_arc_width } },
    { "arc_rounded", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_arc_rounded } },
    { "arc_color", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_arc_color } },
    { "arc_opa", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_arc_opa } },
    { "arc_img_src", SETTER_TYPE_STACK, { .setter_stack_s = _lv_obj_set_style_arc_img_src } },
    { "text_color", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_text_color } },
    { "text_opa", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_text_opa } },
    { "text_font", SETTER_TYPE_POINTER, { .setter_pointer_s = (setter_s_pointer_t)lv_obj_set_style_text_font } },
    { "text_letter_space", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_text_letter_space } },
    { "text_line_space", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_text_line_space } },
    { "text_decor", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_text_decor } },
    { "text_align", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_text_align } },
    { "radius", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_radius } },
    { "clip_corner", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_clip_corner } },
    { "opa", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_opa } },
    // { "color_filter_dsc", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_color_filter_dsc } },
    { "color_filter_opa", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_color_filter_opa } },
    { "anim_time", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_anim_time } },
    { "anim_speed", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_anim_speed } },
    // { "transition", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_transition } },
    { "blend_mode", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_blend_mode } },
    { "layout", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_layout } },
    { "base_dir", SETTER_TYPE_INT, { .setter_s = (setter_s_int_t)lv_obj_set_style_base_dir } },
};
/* clang-format on */

static int obj_set_style_cb(lua_State *L, void *data)
{
  struct obj_style_selector *obj_style = data;

  int ret = lugl_set_obj_style(L, obj_style->obj, obj_style->selector,
                               obj_style_table);

  if (ret != 0) {
    debug("failed\n");
  }

  return ret;
}

/**
 * @brief Set obj properties based on property table on stack top
 */
static inline void lugl_setup_obj(lua_State *L, lv_obj_t *obj)
{
  lugl_iterate(L, -1, obj_set_property_cb, obj);
}

static int lugl_obj_add_userdata(lua_State *L, lv_obj_t *obj)
{
  const char *meta = lugl_class_to_metatable_name(obj);
  debug("create %s: %p\n", meta, obj);
  *(void **)lua_newuserdata(L, sizeof(void *)) = obj;
  luaL_getmetatable(L, meta);
  lua_setmetatable(L, -2);

  /**
   * @todo add userdata to registry using obj as key,
   * to avoid userdata from being gc. Need to confirm this behavior.
   */
  lua_pushlightuserdata(L, obj);
  lua_pushvalue(L, -2);
  lua_rawset(L, LUA_REGISTRYINDEX);

  return 1;
}

/**
 * optional arg:
 *
 * @arg property: initial object property which set directly to obj. Style can
 * be added later
 *
 * root = lvgl.Object()
 * root = lvgl.Object{x = 0, y = 0, w = 100, h = 100}
 * img = root:Image() -- create image on root
 * img = root:Image{x = 0, y = 0, bg_color = 0x00ff00}
 *
 */

static int lugl_obj_create_helper(lua_State *L,
                                  lv_obj_t *(*create)(lv_obj_t *parent))
{
  lugl_ctx_t *ctx = lugl_context(L);
  lv_obj_t *parent;

  if (lua_isnoneornil(L, 1)) {
    parent = ctx->root;
  } else {
    parent = lugl_check_obj(L, 1);
    parent = parent ? parent : ctx->root;
  }

  /* remove parent, in order to keep clean stack to call obj.set */
  lua_remove(L, 1);

  lv_obj_t *obj = create(parent);
  lugl_obj_add_userdata(L, obj);

  if (!lua_istable(L, -2)) {
    /* no need to call setup */
    return 1;
  }

  lua_insert(L, -2); /* obj, prop */

  /* now call obj.set to setup property */
  lua_getfield(L, -2, "set"); /* obj, prop, set */
  if (!lugl_is_callable(L, -1)) {
    /* set method not found, call basic obj set method. */
    lua_pop(L, 2); /* remove prop table, and set method */
    lugl_setup_obj(L, obj);
  } else {
    lua_insert(L, -3); /* set, obj, prop */
    lua_call(L, 2, 0); /* now stack is clean */
    lua_pushlightuserdata(L, obj);
    lua_rawget(L, LUA_REGISTRYINDEX);
  }

  debug("create obj: %p\n", obj);
  return 1;
}

static int lugl_obj_create(lua_State *L)
{
  return lugl_obj_create_helper(L, lv_obj_create);
}

static int lugl_obj_delete(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, -1); /* could reentry, so use -1 */
  if (obj == NULL)
    return 0;

  uint32_t cnt = lv_obj_get_child_cnt(obj);

  for (int i = cnt - 1; i >= 0; i--) {
    lv_obj_t *child = lv_obj_get_child(obj, i);

    lua_checkstack(L, 2);
    lua_pushlightuserdata(L, child);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lugl_obj_delete(L);
  }

  lua_checkstack(L, 2);
  /* remove userdata from registry. */
  lua_pushlightuserdata(L, obj);
  lua_pushnil(L);
  lua_rawset(L, LUA_REGISTRYINDEX);

  lugl_obj_remove_all_anim_int(L, obj);

  bool non_native = true;
  lugl_obj_data_t *data = obj->user_data;

  /* children is already been removed. */
  if (data) {
    non_native = data->non_native;
    lugl_obj_remove_event_all(L, obj);
    free(data);
  }

  if (non_native) {
    lv_obj_del(obj);
  }

  void **udata = lua_touserdata(L, 1);
  *udata = NULL;

  lua_pop(L, 1); /* remove the userdata para */
  return 0;
}

static int lugl_obj_clean(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, -1);
  if (obj == NULL)
    return 0;

  uint32_t cnt = lv_obj_get_child_cnt(obj);

  for (int i = cnt - 1; i >= 0; i--) {
    lv_obj_t *child = lv_obj_get_child(obj, i);

    lua_checkstack(L, 2);
    lua_pushlightuserdata(L, child);
    lua_rawget(L, LUA_REGISTRYINDEX);
    lugl_obj_delete(L);
  }

  lua_pop(L, 1); /* remove the userdata para */
  return 0;
}

/**
 * set the property of object like x, y, w, h etc.
 * #1: obj: lightuserdata
 * #2: {k = v} key: string, choose from x, y, w, h, value: any
 */
static int lugl_obj_set(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been delted.");
    return 0;
  }

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  lugl_setup_obj(L, obj);
  return 0;
}

/**
 * obj:set_style(0, {x = 0, y = 0, bg_opa = 123})
 *
 */
static int lugl_obj_set_style(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been delted.");
    return 0;
  }

  if (!lua_istable(L, 2)) {
    luaL_argerror(L, 2, "expect a table on 2nd para.");
    return 0;
  }

  int selector = 0;
  if (!lua_isnoneornil(L, 3)) {
    selector = lua_tointeger(L, 3);
    lua_pop(L, 1); /* later we use stack[-1] to get table. */
  }

  if (!lua_istable(L, 2)) {
    luaL_argerror(L, 2, "expect a table on 2nd para.");
    return 0;
  }

  struct obj_style_selector obj_style = {.obj = obj, .selector = selector};

  lugl_iterate(L, -1, obj_set_style_cb, &obj_style);
  return 0;
}

static int lugl_obj_set_parent(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been delted.");
    return 0;
  }

  lv_obj_t *parent = lugl_check_obj(L, 2);
  if (parent == NULL) {
    luaL_argerror(L, 1, "obj could already been delted.");
    return 0;
  }

  lv_obj_set_parent(obj, parent);
  return 0;
}

static int lugl_obj_get_screen(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been delted.");
    return 0;
  }

  lv_obj_t *screen = lv_obj_get_screen(obj);

  /* check if userdata is added to this obj, so lua can access it. */
  lua_pushlightuserdata(L, screen);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (lua_isnil(L, -1)) {
    if (screen->user_data) {
      /* oops, user_data has been used by native code */
      return lugl_error(L,
                        "Cannot return native object that user_data is used.");
    }

    /**
     * obj has no userdata, add one.
     * @todo memleak here, since we'll never known when it's deleted.
     *
     * This obj should not be gc'ed by lua, since it's not created by
     * lua.
     */
    lugl_obj_data_t *data = lugl_obj_alloc_data(L, screen);
    data->non_native = true;
    return lugl_obj_add_userdata(L, screen);
  }

  return 1;
}

static int lugl_obj_get_state(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been delted.");
    return 0;
  }

  lv_state_t state = lv_obj_get_state(obj);
  lua_pushinteger(L, state);

  return 1;
}

/**
 * obj:scroll_to({x=10})
 * obj:scroll_to({x=10, anim=true})
 * obj:scroll_to({x=10, y=100, anim=false})
 */
static int lugl_obj_scroll_to(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been delted.");
    return 0;
  }

  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "should be table {x=0,y=0,anim=true}");
  }

  lua_getfield(L, -1, "anim");
  bool anim = lua_toboolean(L, -1);

  lv_coord_t v;
  lua_getfield(L, -1, "x");
  if (!lua_isnil(L, -1)) {
    v = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lv_obj_scroll_to_x(obj, v, anim ? LV_ANIM_ON : LV_ANIM_OFF);
  }

  lua_getfield(L, -1, "y");
  if (!lua_isnil(L, -1)) {
    v = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lv_obj_scroll_to_x(obj, v, anim ? LV_ANIM_ON : LV_ANIM_OFF);
  }

  return 0;
}

static int lugl_obj_is_scrolling(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been delted.");
    return 0;
  }

  lua_pushboolean(L, lv_obj_is_scrolling(obj));

  return 1;
}

static int lugl_obj_is_visible(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been delted.");
    return 0;
  }

  lua_pushboolean(L, lv_obj_is_visible(obj));

  return 1;
}

static int lugl_obj_add_flag(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been delted.");
    return 0;
  }

  lv_obj_flag_t flag = lua_tointeger(L, 2);
  lv_obj_add_flag(obj, flag);

  return 0;
}

static int lugl_obj_clear_flag(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "obj could already been delted.");
    return 0;
  }

  lv_obj_flag_t flag = lua_tointeger(L, 2);
  lv_obj_clear_flag(obj, flag);

  return 0;
}

static int lugl_obj_gc(lua_State *L)
{
  debug("\n");

  lv_obj_t *obj = lugl_check_obj(L, 1);
  debug("GC for obj: %p\n", obj);

  if (obj) {
    lugl_obj_delete(L);
  }

  return 0;
}

static int lugl_obj_tostring(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  const char *meta = lugl_class_to_metatable_name(obj);
  lua_pushfstring(L, "%s: object %p", meta, obj);
  return 1;
}
