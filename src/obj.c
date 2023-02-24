#include <lauxlib.h>
#include <lua.h>

#include "lugl.h"
#include "private.h"
#include <lvgl.h>
#include <stdlib.h>

/* extended feature for object */
#include "event.c"
#include "style.c"
#include "widgets/widgets.c"

static int lugl_anim_create(lua_State *L);
static int lugl_anims_create(lua_State *L);
static int lugl_obj_remove_all_anim(lua_State *L);
static int lugl_obj_delete(lua_State *L);

static void _lv_obj_set_align(void *obj, lua_State *L)
{
  if (lua_isinteger(L, -1)) {
    lv_obj_align(obj, lua_tointeger(L, -1), 0, 0);
    return;
  }

  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "should be table.");
    debug("para should be table.");
    return;
  }

  lua_getfield(L, -1, "type");
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

/**
 * @brief Set obj properties based on property table on stack top
 *
 * Internally used.
 */
static inline void lugl_setup_obj(lua_State *L, lv_obj_t *obj)
{
  lugl_iterate(L, -1, lugl_obj_set_property_kv, obj);
}

static void obj_delete_cb(lv_event_t *e)
{
  lua_State *L = e->user_data;
  lua_pushlightuserdata(L, e->current_target);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (lua_isnoneornil(L, -1)) {
    goto pop_exit;
  }

  lugl_obj_t *lobj = lugl_to_lobj(L, -1);
  if (lobj->lua_created)
    goto pop_exit;

  lugl_obj_delete(L);
  return;

pop_exit:
  lua_pop(L, 1);
  return;
}

/**
 * get the obj userdata and uservalue, if uservalue is not a table, then add
 * one. result stack: table(from uservalue) return succeeded or not
 */
static lugl_obj_t *lugl_obj_touserdatauv(lua_State *L, int idx)
{
  lugl_obj_t *lobj = lugl_to_lobj(L, idx);

  int type = lua_getuservalue(L, idx);
  if (type == LUA_TTABLE)
    return lobj;

  /* initial element: 1 */
  lua_createtable(L, 0, 1);

#if 0 /* reserved slot, not used for now */
  lua_pushinteger(L, 1);
  lua_pushnil(L);
  lua_rawset(L, -3);
#endif
  lua_pushvalue(L, -1); /* leave one on stack */
  lua_setuservalue(L, idx > 0 ? idx : idx - 2);
  return lobj;
}

static int lugl_obj_create(lua_State *L)
{
  return lugl_obj_create_helper(L, lv_obj_create);
}

static int lugl_obj_delete(lua_State *L)
{
  lugl_obj_t *lobj;

  /**
   * Some widget may create child obj that doesn't belong to lua. Ignore them
   * and report no error.
   */
  if (!(lobj = lua_touserdata(L, -1))) {
    return 0;
  }

  if (lobj->obj == NULL) {
    /* could be already deleted, but not gc'ed */
    return 0;
  }

  uint32_t cnt = lv_obj_get_child_cnt(lobj->obj);

  for (int i = cnt - 1; i >= 0; i--) {
    lv_obj_t *child = lv_obj_get_child(lobj->obj, i);
    lua_checkstack(L, 2);
    lua_pushlightuserdata(L, child);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_isnoneornil(L, -1)) {
      continue;
    }

    lugl_obj_delete(L);
  }

  lugl_obj_remove_all_anim_int(L, lobj);
  lugl_obj_remove_event_all(L, lobj);

  /* delete obj firstly, then cleanup memory */
  if (lobj->lua_created) {
    lv_obj_del(lobj->obj);
  }

  lua_checkstack(L, 2);
  /* remove userdata from registry. */
  lua_pushlightuserdata(L, lobj->obj);
  lua_pushnil(L);
  lua_rawset(L, LUA_REGISTRYINDEX);

  debug("delete obj: %p\n", lobj->obj);
  lobj->obj = NULL;

  lua_pop(L, 1); /* remove the userdata para */
  return 0;
}

static int lugl_obj_clean(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, -1);
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

static int lugl_obj_set(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
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
 * obj:align_to({base=base, type=type, x_ofs=0, y_ofs=0})
 */
static int lugl_obj_align_to(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    return luaL_argerror(L, 1, "null obj");
  }

  if (!lua_istable(L, 2)) {
    debug("para should be table.");
    return luaL_argerror(L, 2, "should be table.");
  }

  lua_getfield(L, 2, "type");
  lv_align_t align = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, 2, "base");
  lv_obj_t *base = lugl_to_lobj(L, -1)->obj;
  lua_pop(L, 1);
  if (base == NULL) {
    debug("base is not lua obj");
    return luaL_argerror(L, -1, "base is not lua obj");
  }

  lua_getfield(L, 2, "x_ofs");
  lv_coord_t x_ofs = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, 2, "y_ofs");
  lv_coord_t y_ofs = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lv_obj_align_to(obj, base, align, x_ofs, y_ofs);
  return 0;
}

static int lugl_obj_set_parent(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_obj_t *parent = lugl_to_obj(L, 2);
  if (parent == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_obj_set_parent(obj, parent);
  return 0;
}

static int lugl_obj_get_screen(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_obj_t *screen = lv_obj_get_screen(obj);

  /* check if userdata is added to this obj, so lua can access it. */
  lua_pushlightuserdata(L, screen);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnil(L, -1)) {
    /* create lugl object and attach screen on it. */
    lugl_obj_t *lobj = lugl_add_lobj(L, screen);
    /* mark it's non-lua created, thus cannot be deleted by lua */
    lobj->lua_created = false;
  }

  return 1;
}

static int lugl_obj_get_parent(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_obj_t *parent = lv_obj_get_parent(obj);

  /* check if userdata is added to this obj, so lua can access it. */
  lua_pushlightuserdata(L, parent);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (lua_isnil(L, -1)) {
    /* create lugl object and attach screen on it. */
    lugl_obj_t *lobj = lugl_add_lobj(L, parent);
    /* mark it's non-lua created, thus cannot be deleted by lua */
    lobj->lua_created = false;
  }

  return 1;
}

static int lugl_obj_set_get_parent(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  if (!lua_isnoneornil(L, 2)) {
    lv_obj_t *parent = lugl_to_obj(L, 2);
    if (parent == NULL) {
      luaL_argerror(L, 1, "null obj");
      return 0;
    }
  }

  return lugl_obj_get_parent(L);
}

static int lugl_obj_get_child(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  int id = lugl_tointeger(L, 2);
  lv_obj_t *child = lv_obj_get_child(obj, id);
  if (child == NULL) {
    lua_pushnil(L);
    return 1;
  }

  /* check if userdata is added to this obj, so lua can access it. */
  lua_pushlightuserdata(L, child);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (lua_isnil(L, -1)) {
    lugl_add_lobj(L, child)->lua_created = false;
  }

  return 1;
}

static int lugl_obj_get_child_cnt(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lua_pushinteger(L, lv_obj_get_child_cnt(obj));
  return 1;
}

static int lugl_obj_get_state(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
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
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "should be table {x=0,y=0,anim=true}");
  }

  lua_getfield(L, -1, "anim");
  bool anim = lugl_tointeger(L, -1);
  lua_pop(L, 1);

  lv_coord_t v;
  lua_getfield(L, -1, "x");
  if (!lua_isnil(L, -1)) {
    v = lua_tointeger(L, -1);
    lv_obj_scroll_to_x(obj, v, anim ? LV_ANIM_ON : LV_ANIM_OFF);
  }
  lua_pop(L, 1);

  lua_getfield(L, -1, "y");
  if (!lua_isnil(L, -1)) {
    v = lua_tointeger(L, -1);
    lv_obj_scroll_to_x(obj, v, anim ? LV_ANIM_ON : LV_ANIM_OFF);
  }
  lua_pop(L, 1);

  return 0;
}

static int lugl_obj_is_visible(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lua_pushboolean(L, lv_obj_is_visible(obj));

  return 1;
}

static int lugl_obj_add_flag(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_obj_flag_t flag = lua_tointeger(L, 2);
  lv_obj_add_flag(obj, flag);

  return 0;
}

static int lugl_obj_clear_flag(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_obj_flag_t flag = lua_tointeger(L, 2);
  lv_obj_clear_flag(obj, flag);

  return 0;
}

static int lugl_obj_add_state(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_state_t state = lua_tointeger(L, 2);
  lv_obj_add_state(obj, state);
  return 0;
}

static int lugl_obj_clear_state(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_state_t state = lua_tointeger(L, 2);
  lv_obj_clear_state(obj, state);
  return 0;
}

/**
 * obj:scroll_by(x, y, anim_en)
 */
static int lugl_obj_scroll_by(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  int x = lugl_tointeger(L, 2);
  int y = lugl_tointeger(L, 3);
  int anim_en = lugl_tointeger(L, 4);

  lv_obj_scroll_by(obj, x, y, anim_en);
  return 0;
}

static int lugl_obj_scroll_by_bounded(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  int dx = lugl_tointeger(L, 2);
  int dy = lugl_tointeger(L, 3);
  int anim_en = lugl_tointeger(L, 4);

  lv_obj_scroll_by_bounded(obj, dx, dy, anim_en);
  return 0;
}

static int lugl_obj_scroll_to_view(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  int anim_en = lugl_tointeger(L, 2);

  lv_obj_scroll_to_view(obj, anim_en);
  return 0;
}

static int lugl_obj_scroll_to_view_recursive(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  int anim_en = lugl_tointeger(L, 2);

  lv_obj_scroll_to_view_recursive(obj, anim_en);
  return 0;
}

static int lugl_obj_scroll_by_raw(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  int x = lugl_tointeger(L, 2);
  int y = lugl_tointeger(L, 3);

  _lv_obj_scroll_by_raw(obj, x, y);
  return 0;
}

static int lugl_obj_is_scrolling(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lua_pushboolean(L, lv_obj_is_scrolling(obj));
  return 1;
}

static int lugl_obj_scrollbar_invalidate(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_obj_scrollbar_invalidate(obj);
  return 0;
}

static int lugl_obj_readjust_scroll(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  int anim_en = lugl_tointeger(L, 2);
  lv_obj_readjust_scroll(obj, anim_en);
  return 0;
}

static int lugl_obj_is_editable(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lua_pushboolean(L, lv_obj_is_editable(obj));
  return 1;
}

static int lugl_obj_is_group_def(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lua_pushboolean(L, lv_obj_is_group_def(obj));
  return 1;
}

static int lugl_obj_is_layout_positioned(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lua_pushboolean(L, lv_obj_is_layout_positioned(obj));
  return 1;
}

static int lugl_obj_mark_layout_as_dirty(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_obj_mark_layout_as_dirty(obj);
  return 0;
}

static int lugl_obj_center(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_obj_center(obj);
  return 0;
}

static int lugl_obj_invalidate(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_obj_invalidate(obj);
  return 0;
}

static int lugl_obj_set_flex_flow(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_flex_flow_t flow = lugl_tointeger(L, 2);

  lv_obj_set_flex_flow(obj, flow);
  return 0;
}

static int lugl_obj_set_flex_align(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_flex_align_t m = lugl_tointeger(L, 2);
  lv_flex_align_t c = lugl_tointeger(L, 3);
  lv_flex_align_t t = lugl_tointeger(L, 4);

  lv_obj_set_flex_align(obj, m, c, t);
  return 0;
}

static int lugl_obj_set_flex_grow(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  uint8_t grow = lugl_tointeger(L, 2);

  lv_obj_set_flex_grow(obj, grow);
  return 0;
}

static int lugl_obj_indev_search(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (obj == NULL) {
    luaL_argerror(L, 1, "null obj");
    return 0;
  }

  lv_point_t point;
  if (lua_istable(L, 2)) {
    lua_geti(L, 2, 1);
    point.x = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_geti(L, 2, 2);
    point.y = lua_tointeger(L, -1);
    lua_pop(L, 1);
  } else {
    point.x = lua_tointeger(L, 2);
    point.y = lua_tointeger(L, 3);
  }

  obj = lv_indev_search_obj(obj, &point);
  if (obj == NULL) {
    lua_pushnil(L);
  } else {
    lua_pushlightuserdata(L, obj);
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_isnoneornil(L, -1)) {
      lugl_add_lobj(L, obj)->lua_created = false;
    }
  }

  return 1;
}

static int lugl_obj_gc(lua_State *L)
{
  if (lua_type(L, 1) != LUA_TUSERDATA) {
    /* If t = setmetatable({}, obj_meta_table), this will happen when t is
     * gc;ed. Currently all metatables for classes based on obj, that has no own
     * __gc will goes here.
     */
    return 0;
  }

  debug("\n");

  lv_obj_t *obj = lugl_to_obj(L, 1);
  debug("GC for obj: %p\n", obj);

  if (obj) {
    lugl_obj_delete(L);
  }

  return 0;
}

static const luaL_Reg lugl_obj_methods[] = {
    {"set",                      lugl_obj_set                     },
    {"set_style",                lugl_obj_set_style               },
    {"align_to",                 lugl_obj_align_to                },
    {"delete",                   lugl_obj_delete                  },
    {"clean",                    lugl_obj_clean                   },

 /* misc. functions */
    {"parent",                   lugl_obj_set_get_parent          },
    {"set_parent",               lugl_obj_set_parent              },
    {"get_parent",               lugl_obj_get_parent              },
    {"get_child",                lugl_obj_get_child               },
    {"get_child_cnt",            lugl_obj_get_child_cnt           },
    {"get_screen",               lugl_obj_get_screen              },
    {"get_state",                lugl_obj_get_state               },
    {"scroll_to",                lugl_obj_scroll_to               },
    {"is_scrolling",             lugl_obj_is_scrolling            },
    {"is_visible",               lugl_obj_is_visible              },
    {"add_flag",                 lugl_obj_add_flag                },
    {"clear_flag",               lugl_obj_clear_flag              },
    {"add_state",                lugl_obj_add_state               },
    {"clear_state",              lugl_obj_clear_state             },
    {"add_style",                lugl_obj_add_style               },
    {"remove_style",             lugl_obj_remove_style            },
    {"remove_style_all",         lugl_obj_remove_style_all        },
    {"scroll_by",                lugl_obj_scroll_by               },
    {"scroll_by_bounded",        lugl_obj_scroll_by_bounded       },
    {"scroll_to_view",           lugl_obj_scroll_to_view          },
    {"scroll_to_view_recursive", lugl_obj_scroll_to_view_recursive},
    {"scroll_by_raw",            lugl_obj_scroll_by_raw           },
    {"scrollbar_invalidate",     lugl_obj_scrollbar_invalidate    },
    {"readjust_scroll",          lugl_obj_readjust_scroll         },
    {"is_editable",              lugl_obj_is_editable             },
    {"is_group_def",             lugl_obj_is_group_def            },
    {"is_layout_positioned",     lugl_obj_is_layout_positioned    },
    {"mark_layout_as_dirty",     lugl_obj_mark_layout_as_dirty    },
    {"center",                   lugl_obj_center                  },
    {"invalidate",               lugl_obj_invalidate              },
    {"set_flex_flow",            lugl_obj_set_flex_flow           },
    {"set_flex_align",           lugl_obj_set_flex_align          },
    {"set_flex_grow",            lugl_obj_set_flex_grow           },
    {"indev_search",             lugl_obj_indev_search            },

    {"onevent",                  lugl_obj_on_event                },
    {"onPressed",                lugl_obj_on_pressed              },
    {"onClicked",                lugl_obj_on_clicked              },
    {"anim",                     lugl_anim_create                 }, /* in lua, we only support add anim to obj */
    {"anims",                    lugl_anims_create                }, /* create multiple anim */
    {"remove_all_anim",          lugl_obj_remove_all_anim         }, /* remove all */
    {NULL,                       NULL                             },
};

static void lugl_obj_init(lua_State *L)
{
  /* base lv_obj */
  lugl_obj_newmetatable(L, &lv_obj_class, "lv_obj", lugl_obj_methods);
  lua_pushcfunction(L, lugl_obj_gc);
  lua_setfield(L, -2, "__gc");

  /**
   * Widget creation functions is a metatable, so we can add extended widget to
   * it.
   *
   * widgets = {}
   * lib = {widget_create_methods}
   * widgets.__index = lib
   * obj.__metatable = widgets
   */
  lua_getfield(L, -1, "__index");
  luaL_newmetatable(L, "widgets");
  luaL_newlib(L, widget_create_methods);
  lua_setfield(L, -2, "__index");
  lua_setmetatable(L, -2);
  lua_pop(L, 1); /* remove obj.__index table */

  lua_pop(L, 1); /* remove obj metatable */
}

static const lugl_value_setter_t obj_property_table[] = {
    {"x",              0,                 {.setter = (setter_int_t)lv_obj_set_x}             },
    {"y",              0,                 {.setter = (setter_int_t)lv_obj_set_y}             },
    {"w",              0,                 {.setter = (setter_int_t)lv_obj_set_width}         },
    {"h",              0,                 {.setter = (setter_int_t)lv_obj_set_height}        },
    {"align",          SETTER_TYPE_STACK, {.setter_stack = _lv_obj_set_align}                },

    {"scrollbar_mode", 0,                 {.setter = (setter_int_t)lv_obj_set_scrollbar_mode}},
    {"scroll_dir",     0,                 {.setter = (setter_int_t)lv_obj_set_scroll_dir}    },
    {"scroll_snap_x",  0,                 {.setter = (setter_int_t)lv_obj_set_scroll_snap_x} },
    {"scroll_snap_y",  0,                 {.setter = (setter_int_t)lv_obj_set_scroll_snap_y} },
};

/**
 * Set object property.
 * Differ from set object style, this one is usually used to set widget
 * property like lv_WIDGETNAME_set_src()
 *
 * Used internally.
 *
 * Expected stack:
 * stack[-2]: key(property name)
 * stack[-1]: value(could be any lua data)
 */
LUALIB_API int lugl_obj_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = lugl_set_property(L, obj, obj_property_table);

  if (ret == 0)
    return 0;

  /* fallback to set obj local style, with default state. */
  return lugl_obj_set_style_kv(L, obj, 0);
}

LUALIB_API int lugl_obj_create_helper(lua_State *L,
                                      lv_obj_t *(*create)(lv_obj_t *parent))
{
  lugl_ctx_t *ctx = lugl_context(L);
  lv_obj_t *parent;

  if (lua_isnoneornil(L, 1)) {
    parent = ctx->root;
  } else {
    parent = lugl_to_obj(L, 1);
    parent = parent ? parent : ctx->root;
  }

  /* remove parent, in order to keep clean stack to call obj.set */
  lua_remove(L, 1);

  lv_obj_t *obj = create(parent);
  lugl_add_lobj(L, obj)->lua_created = true;

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

/**
 * Add existing lvgl obj to lua, return lobj(lugl obj).
 * If no metatable not found for this obj class, then lv_obj_class metatable is
 * used
 */
LUALIB_API lugl_obj_t *lugl_add_lobj(lua_State *L, lv_obj_t *obj)
{
  lugl_obj_t *lobj;

  lua_pushlightuserdata(L, obj);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (!lua_isnoneornil(L, -1)) {
    /* already exists */
    return lugl_to_lobj(L, -1);
  }
  lua_pop(L, 1); /* pop nil value */

  lobj = lua_newuserdata(L, sizeof(*lobj));

  if (lugl_obj_getmetatable(L, obj->class_p) == LUA_TNIL) {
    lua_pop(L, 1);
    debug("cannot find metatable for class: %p\n", obj->class_p);
    /* use base obj metatable instead */
    lugl_obj_getmetatable(L, &lv_obj_class);
  }

  lua_setmetatable(L, -2);

  memset(lobj, 0, sizeof(*lobj));
  lugl_obj_anim_init(lobj);
  lugl_obj_event_init(lobj);
  lobj->obj = obj;
  lv_obj_add_event_cb(obj, obj_delete_cb, LV_EVENT_DELETE, L);

  /* registry[obj] = lobj */
  lua_pushlightuserdata(L, obj);
  lua_pushvalue(L, -2);
  lua_rawset(L, LUA_REGISTRYINDEX);
  return lobj;
}
