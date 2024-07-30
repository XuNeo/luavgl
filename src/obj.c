#include "luavgl.h"
#include "private.h"

/* extended feature for object */
#include "event.c"
#include "rotable.h"
#include "style.c"
#include "widgets/widgets.c"

static int luavgl_anim_create(lua_State *L);
static int luavgl_obj_delete(lua_State *L);
static int luavgl_obj_clean(lua_State *L);

struct setprop_para {
  lv_obj_t *obj;
  const luavgl_table_t *table;
};

/**
 * @brief Set obj properties based on property table on stack top
 *
 * Internally used.
 */
static inline void luavgl_setup_obj(lua_State *L, lv_obj_t *obj,
                                    const luavgl_table_t *properties)
{
  struct setprop_para para = {
      .obj = obj,
      .table = properties,
  };

  luavgl_iterate(L, -1, luavgl_obj_set_property_kv, &para);
}

static void obj_delete_cb(lv_event_t *e)
{
  lua_State *L = e->user_data;
  lua_pushlightuserdata(L, e->current_target);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (lua_isnoneornil(L, -1)) {
    goto pop_exit;
  }

  luavgl_obj_t *lobj = luavgl_to_lobj(L, -1);
  if (lobj->lua_created)
    goto pop_exit;

  /* Clean its children firstly */
  luavgl_obj_clean(L);

  /* Remove events added from lua, but keep them unremoved */
  int size = lv_array_size(&lobj->events);
  struct event_callback_s *event;
  struct event_callback_s **events = lv_array_front(&lobj->events);
  for (int i = 0; i < size; i++) {
    event = events[i];
    if (event == NULL || event->dsc == NULL) {
      continue;
    }

    luaL_unref(L, LUA_REGISTRYINDEX, event->ref);
    lv_free(event);
  }

  lv_array_deinit(&lobj->events);

  /* remove userdata from registry. */
  lua_checkstack(L, 2);
  lua_pushlightuserdata(L, lobj->obj);
  lua_pushnil(L);
  lua_rawset(L, LUA_REGISTRYINDEX);

  LV_LOG_INFO("delete obj: %p", lobj->obj);
  lobj->obj = NULL;
  return;

pop_exit:
  lua_pop(L, 1);
  return;
}

/**
 * get the obj userdata and uservalue, if uservalue is not a table, then add
 * one. result stack: table(from uservalue)
 * return uservalue type: LUA_TTABLE
 */
LUALIB_API int luavgl_obj_getuserdatauv(lua_State *L, int idx)
{
  int type = lua_getuservalue(L, idx);
  if (type == LUA_TTABLE)
    return type;

  lua_pop(L, 1);
  /* initial element: 1 */
  lua_createtable(L, 0, 1);

#if 0 /* reserved slot, not used for now */
  lua_pushinteger(L, 1);
  lua_pushnil(L);
  lua_rawset(L, -3);
#endif
  lua_pushvalue(L, -1); /* leave one on stack */
  lua_setuservalue(L, idx > 0 ? idx : idx - 3);
  return LUA_TTABLE;
}

static int luavgl_obj_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_obj_create);
}

static int luavgl_obj_delete(lua_State *L)
{
  luavgl_obj_t *lobj;

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

    luavgl_obj_delete(L);
  }

  luavgl_obj_remove_event_all(L, lobj);

  /* delete obj firstly, then cleanup memory */
  if (lobj->lua_created) {
    lv_obj_del(lobj->obj);
  }

  lua_checkstack(L, 2);
  /* remove userdata from registry. */
  lua_pushlightuserdata(L, lobj->obj);
  lua_pushnil(L);
  lua_rawset(L, LUA_REGISTRYINDEX);

  LV_LOG_INFO("delete obj: %p", lobj->obj);
  lobj->obj = NULL;

  lua_pop(L, 1); /* remove the userdata para */
  return 0;
}

static int luavgl_obj_clean(lua_State *L)
{
  luavgl_obj_t *lobj = luavgl_to_lobj(L, -1);
  if (lobj == NULL || lobj->obj == NULL)
    return 0;

  lv_obj_t *obj = lobj->obj;
  uint32_t cnt = lv_obj_get_child_cnt(obj);

  for (int i = cnt - 1; i >= 0; i--) {
    lv_obj_t *child = lv_obj_get_child(obj, i);

    lua_checkstack(L, 2);
    lua_pushlightuserdata(L, child);
    lua_rawget(L, LUA_REGISTRYINDEX);
    luavgl_obj_delete(L);
  }

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_set(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para");
    lua_settop(L, 1);
    return 1;
  }

  const luavgl_table_t *properties;

  int t = lua_getfield(L, 1, "__property");
  if (t != LUA_TLIGHTUSERDATA) {
    return luaL_argerror(L, 1, "has no __property");
  }

  properties = lua_touserdata(L, -1);
  lua_pop(L, 1); /* Get rid of the property userdata from stack */
  luavgl_setup_obj(L, obj, properties);

  lua_settop(L, 1);
  return 1;
}

/**
 * obj:align_to({base=base, type=type, x_ofs=0, y_ofs=0})
 */
static int luavgl_obj_align_to(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, 2)) {
    LV_LOG_ERROR("para should be table");
    return luaL_argerror(L, 2, "should be table");
  }

  lua_getfield(L, 2, "type");
  lv_align_t align = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, 2, "base");
  lv_obj_t *base = luavgl_to_lobj(L, -1)->obj;
  lua_pop(L, 1);
  if (base == NULL) {
    LV_LOG_ERROR("base is not lua obj");
    return luaL_argerror(L, -1, "base is not lua obj");
  }

  lua_getfield(L, 2, "x_ofs");
  lv_coord_t x_ofs = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, 2, "y_ofs");
  lv_coord_t y_ofs = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lv_obj_align_to(obj, base, align, x_ofs, y_ofs);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_set_parent(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_obj_t *parent = luavgl_to_obj(L, 2);
  lv_obj_set_parent(obj, parent);
  lua_pop(L, 1);
  return 1;
}

static int luavgl_obj_get_screen(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_obj_t *screen = lv_obj_get_screen(obj);

  /* check if userdata is added to this obj, so lua can access it. */
  lua_pushlightuserdata(L, screen);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnil(L, -1)) {
    /* create luavgl object and attach screen on it. */
    luavgl_obj_t *lobj = luavgl_add_lobj(L, screen);
    /* mark it's non-lua created, thus cannot be deleted by lua */
    lobj->lua_created = false;
  }

  return 1;
}

static int luavgl_obj_get_parent(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_obj_t *parent = lv_obj_get_parent(obj);

  /* check if userdata is added to this obj, so lua can access it. */
  lua_pushlightuserdata(L, parent);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (lua_isnil(L, -1)) {
    /* create luavgl object and attach screen on it. */
    luavgl_obj_t *lobj = luavgl_add_lobj(L, parent);
    /* mark it's non-lua created, thus cannot be deleted by lua */
    lobj->lua_created = false;
  }

  return 1;
}

static int luavgl_obj_set_get_parent(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  if (!lua_isnoneornil(L, 2)) {
    lv_obj_t *parent = luavgl_to_obj(L, 2);
    lv_obj_set_parent(obj, parent);
  }

  return luavgl_obj_get_parent(L);
}

static int luavgl_obj_get_child(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  int id = luavgl_tointeger(L, 2);
  lv_obj_t *child = lv_obj_get_child(obj, id);
  if (child == NULL) {
    lua_pushnil(L);
    return 1;
  }

  /* check if userdata is added to this obj, so lua can access it. */
  lua_pushlightuserdata(L, child);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (lua_isnil(L, -1)) {
    luavgl_add_lobj(L, child)->lua_created = false;
  }

  return 1;
}

static int luavgl_obj_get_child_cnt(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushinteger(L, lv_obj_get_child_cnt(obj));
  return 1;
}

static int luavgl_obj_get_state(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_state_t state = lv_obj_get_state(obj);
  lua_pushinteger(L, state);

  return 1;
}

/**
 * obj:scroll_to({x=10})
 * obj:scroll_to({x=10, anim=true})
 * obj:scroll_to({x=10, y=100, anim=false})
 */
static int luavgl_obj_scroll_to(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "should be table {x=0,y=0,anim=true}");
  }

  lua_getfield(L, -1, "anim");
  bool anim = luavgl_tointeger(L, -1);
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

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_is_visible(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushboolean(L, lv_obj_is_visible(obj));

  return 1;
}

static int luavgl_obj_add_flag(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_obj_flag_t flag = lua_tointeger(L, 2);
  lv_obj_add_flag(obj, flag);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_clear_flag(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_obj_flag_t flag = lua_tointeger(L, 2);
  lv_obj_clear_flag(obj, flag);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_add_state(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_state_t state = lua_tointeger(L, 2);
  lv_obj_add_state(obj, state);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_clear_state(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_state_t state = lua_tointeger(L, 2);
  lv_obj_clear_state(obj, state);

  lua_settop(L, 1);
  return 1;
}

/**
 * obj:scroll_by(x, y, anim_en)
 */
static int luavgl_obj_scroll_by(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  int x = luavgl_tointeger(L, 2);
  int y = luavgl_tointeger(L, 3);
  int anim_en = luavgl_tointeger(L, 4);

  lv_obj_scroll_by(obj, x, y, anim_en);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_scroll_by_bounded(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  int dx = luavgl_tointeger(L, 2);
  int dy = luavgl_tointeger(L, 3);
  int anim_en = luavgl_tointeger(L, 4);

  lv_obj_scroll_by_bounded(obj, dx, dy, anim_en);
  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_scroll_to_view(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  int anim_en = luavgl_tointeger(L, 2);

  lv_obj_scroll_to_view(obj, anim_en);
  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_scroll_to_view_recursive(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  int anim_en = luavgl_tointeger(L, 2);

  lv_obj_scroll_to_view_recursive(obj, anim_en);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_scroll_by_raw(lua_State *L)
{
#if LV_VERSION_CHECK(8, 3, 0)
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  int x = luavgl_tointeger(L, 2);
  int y = luavgl_tointeger(L, 3);

  _lv_obj_scroll_by_raw(obj, x, y);
#endif

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_is_scrolling(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushboolean(L, lv_obj_is_scrolling(obj));
  return 1;
}

static int luavgl_obj_scrollbar_invalidate(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_obj_scrollbar_invalidate(obj);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_readjust_scroll(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  int anim_en = luavgl_tointeger(L, 2);
  lv_obj_readjust_scroll(obj, anim_en);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_is_editable(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushboolean(L, lv_obj_is_editable(obj));
  return 1;
}

static int luavgl_obj_is_group_def(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushboolean(L, lv_obj_is_group_def(obj));
  return 1;
}

static int luavgl_obj_is_layout_positioned(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushboolean(L, lv_obj_is_layout_positioned(obj));
  return 1;
}

static int luavgl_obj_mark_layout_as_dirty(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_obj_mark_layout_as_dirty(obj);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_center(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_obj_center(obj);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_invalidate(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_obj_invalidate(obj);

  return 1;
}

static int luavgl_obj_set_flex_flow(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_flex_flow_t flow = luavgl_tointeger(L, 2);

  lv_obj_set_flex_flow(obj, flow);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_set_flex_align(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_flex_align_t m = luavgl_tointeger(L, 2);
  lv_flex_align_t c = luavgl_tointeger(L, 3);
  lv_flex_align_t t = luavgl_tointeger(L, 4);

  lv_obj_set_flex_align(obj, m, c, t);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_set_flex_grow(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  uint8_t grow = luavgl_tointeger(L, 2);

  lv_obj_set_flex_grow(obj, grow);

  lua_settop(L, 1);
  return 1;
}

static int luavgl_obj_indev_search(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
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
      luavgl_add_lobj(L, obj)->lua_created = false;
    }
  }

  return 1;
}

static int luavgl_obj_get_coords(lua_State *L)
{
  lv_area_t area;
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_obj_get_coords(obj, &area);
  lua_newtable(L);

  lua_pushinteger(L, area.x1);
  lua_setfield(L, -2, "x1");

  lua_pushinteger(L, area.x2);
  lua_setfield(L, -2, "x2");

  lua_pushinteger(L, area.y1);
  lua_setfield(L, -2, "y1");

  lua_pushinteger(L, area.y2);
  lua_setfield(L, -2, "y2");

  return 1;
}

/**
 * get object real position using lv_obj_get_x/x2/y/y2
 */
static int luavgl_obj_get_pos(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_newtable(L);

  lua_pushinteger(L, lv_obj_get_x(obj));
  lua_setfield(L, -2, "x1");

  lua_pushinteger(L, lv_obj_get_x2(obj));
  lua_setfield(L, -2, "x2");

  lua_pushinteger(L, lv_obj_get_y(obj));
  lua_setfield(L, -2, "y1");

  lua_pushinteger(L, lv_obj_get_y2(obj));
  lua_setfield(L, -2, "y2");

  return 1;
}

/**
 * Remove all animations associates to this object
 */
static int luavgl_obj_remove_anim_all(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_anim_del(obj, NULL);
  return 1;
}

static int luavgl_obj_gc(lua_State *L)
{
  if (lua_type(L, 1) != LUA_TUSERDATA) {
    /* If t = setmetatable({}, obj_meta_table), this will happen when t is
     * gc;ed. Currently all metatables for classes based on obj, that has no own
     * __gc will goes here.
     */
    return 0;
  }

  LV_LOG_INFO("enter");

  luavgl_obj_t *lobj = lua_touserdata(L, 1);
  if (lobj == NULL || lobj->obj == NULL) {
    /* obj is already deleted. It's ok. */
    return 0;
  }

  LV_LOG_INFO("GC for obj: %p", lobj->obj);
  luavgl_obj_delete(L);

  return 0;
}

/**
 * Set object property. The property is distinguished by name, the same name
 * such as width is both a widget property and style. We treat widget property
 * has higher priority than style. For some special property, even lvgl has
 * corresponding property API, the value is not easy for lua to deal with,
 * these properties are also handled by lua callback directly.
 *
 * On stack, -2: property name, -1: propery value
 */
static int obj_set_property_stack(lua_State *L, lv_obj_t *obj,
                                  const luavgl_table_t *table)
{
  const char *key = lua_tostring(L, -2);

  if (table && table->array) {
    const luavgl_property_ops_t *ops = table->array;

    /* 1. Try the luavgl property table for this specific class. */
    for (int i = 0; i < table->len; i++) {
      if (lv_strcmp(ops[i].name, key) == 0) {
        return ops[i].ops(L, obj, true);
      }
    }
  }

  /* 2. Try lvgl property */
  lv_prop_id_t id = lv_obj_property_get_id(obj, key);
  if (id != LV_PROPERTY_ID_INVALID) {
    lv_property_t prop = luavgl_toproperty(L, -1, id);
    if (prop.id != LV_PROPERTY_ID_INVALID) {
      if (lv_obj_set_property(obj, &prop) == LV_RESULT_OK) {
        return 0;
      }
    }
  }

  /* 3. Try luavgl obj style */
  if (luavgl_obj_set_style_kv(L, obj, LV_STATE_DEFAULT) == 0) {
    return 0;
  }

  LV_LOG_ERROR("property not found: %s", key);
  return luaL_error(L, "property not found: %s", key);
}

static int obj_get_property_stack(lua_State *L, lv_obj_t *obj,
                                  const luavgl_table_t *table)
{
  const char *key = lua_tostring(L, -1);

  if (table && table->array) {
    const luavgl_property_ops_t *ops = table->array;

    /* 1. Try the luavgl property table for this specific class. */
    for (int i = 0; i < table->len; i++) {
      if (lv_strcmp(ops[i].name, key) == 0) {
        return ops[i].ops(L, obj, false);
      }
    }
  }

  /* 2. Try lvgl property */
  lv_prop_id_t id = lv_obj_property_get_id(obj, key);
  lv_property_t prop = {};
  if (id != LV_PROPERTY_ID_INVALID) {
    prop = lv_obj_get_property(obj, id);
    if (prop.id != LV_PROPERTY_ID_INVALID) {
      return luavgl_pushproperty(L, &prop);
    }
  }

  /* 3. Try luavgl obj style */

  LV_LOG_ERROR("property not found: %s", key);
  return luaL_error(L, "property not found: %s", key);
}

/**
 * Check if the property is supported by luavgl, if not then try base class.
 * If luavgl doesn't support the property, then check lvgl property.
 *
 * In this way, luavgl property method can override lvgl property method.
 *
 * On stack, 1: obj, 2: property name, 3: propery value
 */
LUALIB_API int luavgl_obj_set_property(lua_State *L)
{
  int t;
  const luavgl_table_t *table;
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  t = lua_getfield(L, 1, "__property");
  if (t != LUA_TLIGHTUSERDATA) {
    return luaL_argerror(L, 1, "has no __property");
  }

  table = lua_touserdata(L, -1);
  lua_pop(L, 1); /* Get rid of the property userdata from stack */

  return obj_set_property_stack(L, obj, table);
}

LUALIB_API int luavgl_obj_get_property(lua_State *L)
{
  int t;
  const luavgl_table_t *table;
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  t = lua_getfield(L, 1, "__property");
  if (t != LUA_TLIGHTUSERDATA) {
    return luaL_argerror(L, 1, "has no __property");
  }

  table = lua_touserdata(L, -1);
  lua_pop(L, 1); /* Get rid of the property userdata from stack */

  return obj_get_property_stack(L, obj, table);
}

static int obj_property_align(lua_State *L, lv_obj_t *obj, bool set)
{
  if (set) {
    if (lua_isinteger(L, -1)) {
      lv_obj_align(obj, lua_tointeger(L, -1), 0, 0);
      return 0;
    }

    if (!lua_istable(L, -1)) {
      LV_LOG_ERROR("para should be table");
      luaL_argerror(L, -1, "should be table");
      return 0;
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

    return 0;
  } else {
    /* get property */
    lua_pushinteger(L, lv_obj_get_x(obj));
    return 1;
  }
}

static int obj_property_w(lua_State *L, lv_obj_t *obj, bool set)
{
  if (set) {
    lv_obj_set_width(obj, lua_tointeger(L, -1));
  } else {
    /* get property */
    lua_pushinteger(L, lv_obj_get_width(obj));
  }
  return 1;
}

static int obj_property_h(lua_State *L, lv_obj_t *obj, bool set)
{
  if (set) {
    lv_obj_set_height(obj, lua_tointeger(L, -1));
  } else {
    /* get property */
    lua_pushinteger(L, lv_obj_get_height(obj));
  }
  return 1;
}

static const luavgl_property_ops_t obj_property_ops[] = {
    {.name = "align", .ops = obj_property_align},
    {.name = "h",     .ops = obj_property_h    },
    {.name = "w",     .ops = obj_property_w    },
};

static const luavgl_table_t property_table = {
    .len = sizeof(obj_property_ops) / sizeof(obj_property_ops[0]),
    .array = obj_property_ops,
};

static const rotable_Reg luavgl_obj_methods[] = {
    {"set",                      LUA_TFUNCTION,      {luavgl_obj_set}                     },
    {"get",                      LUA_TFUNCTION,      {luavgl_obj_get_property}            },

    {"testprop",                 LUA_TFUNCTION,      {luavgl_obj_set_property}            },
    {"set_style",                LUA_TFUNCTION,      {luavgl_obj_set_style}               },
    {"align_to",                 LUA_TFUNCTION,      {luavgl_obj_align_to}                },
    {"delete",                   LUA_TFUNCTION,      {luavgl_obj_delete}                  },
    {"clean",                    LUA_TFUNCTION,      {luavgl_obj_clean}                   },
    /* misc. functions */
    {"parent",                   LUA_TFUNCTION,      {luavgl_obj_set_get_parent}          },
    {"set_parent",               LUA_TFUNCTION,      {luavgl_obj_set_parent}              },
    {"get_parent",               LUA_TFUNCTION,      {luavgl_obj_get_parent}              },
    {"get_child",                LUA_TFUNCTION,      {luavgl_obj_get_child}               },
    {"get_child_cnt",            LUA_TFUNCTION,      {luavgl_obj_get_child_cnt}           },
    {"get_screen",               LUA_TFUNCTION,      {luavgl_obj_get_screen}              },
    {"get_state",                LUA_TFUNCTION,      {luavgl_obj_get_state}               },
    {"scroll_to",                LUA_TFUNCTION,      {luavgl_obj_scroll_to}               },
    {"is_scrolling",             LUA_TFUNCTION,      {luavgl_obj_is_scrolling}            },
    {"is_visible",               LUA_TFUNCTION,      {luavgl_obj_is_visible}              },
    {"add_flag",                 LUA_TFUNCTION,      {luavgl_obj_add_flag}                },
    {"clear_flag",               LUA_TFUNCTION,      {luavgl_obj_clear_flag}              },
    {"add_state",                LUA_TFUNCTION,      {luavgl_obj_add_state}               },
    {"clear_state",              LUA_TFUNCTION,      {luavgl_obj_clear_state}             },
    {"add_style",                LUA_TFUNCTION,      {luavgl_obj_add_style}               },
    {"remove_style",             LUA_TFUNCTION,      {luavgl_obj_remove_style}            },
    {"remove_style_all",         LUA_TFUNCTION,      {luavgl_obj_remove_style_all}        },
    {"scroll_by",                LUA_TFUNCTION,      {luavgl_obj_scroll_by}               },
    {"scroll_by_bounded",        LUA_TFUNCTION,      {luavgl_obj_scroll_by_bounded}       },
    {"scroll_to_view",           LUA_TFUNCTION,      {luavgl_obj_scroll_to_view}          },
    {"scroll_to_view_recursive",
     LUA_TFUNCTION,                                  {luavgl_obj_scroll_to_view_recursive}},
    {"scroll_by_raw",            LUA_TFUNCTION,      {luavgl_obj_scroll_by_raw}           },
    {"scrollbar_invalidate",     LUA_TFUNCTION,      {luavgl_obj_scrollbar_invalidate}    },
    {"readjust_scroll",          LUA_TFUNCTION,      {luavgl_obj_readjust_scroll}         },
    {"is_editable",              LUA_TFUNCTION,      {luavgl_obj_is_editable}             },
    {"is_group_def",             LUA_TFUNCTION,      {luavgl_obj_is_group_def}            },
    {"is_layout_positioned",     LUA_TFUNCTION,      {luavgl_obj_is_layout_positioned}    },
    {"mark_layout_as_dirty",     LUA_TFUNCTION,      {luavgl_obj_mark_layout_as_dirty}    },
    {"center",                   LUA_TFUNCTION,      {luavgl_obj_center}                  },
    {"invalidate",               LUA_TFUNCTION,      {luavgl_obj_invalidate}              },
    {"set_flex_flow",            LUA_TFUNCTION,      {luavgl_obj_set_flex_flow}           },
    {"set_flex_align",           LUA_TFUNCTION,      {luavgl_obj_set_flex_align}          },
    {"set_flex_grow",            LUA_TFUNCTION,      {luavgl_obj_set_flex_grow}           },
    {"indev_search",             LUA_TFUNCTION,      {luavgl_obj_indev_search}            },
    {"get_coords",               LUA_TFUNCTION,      {luavgl_obj_get_coords}              },
    {"get_pos",                  LUA_TFUNCTION,      {luavgl_obj_get_pos}                 },

    {"onevent",                  LUA_TFUNCTION,      {luavgl_obj_on_event}                },
    {"onPressed",                LUA_TFUNCTION,      {luavgl_obj_on_pressed}              },
    {"onClicked",                LUA_TFUNCTION,      {luavgl_obj_on_clicked}              },
    {"onShortClicked",           LUA_TFUNCTION,      {luavgl_obj_on_short_clicked}        },
    {"anim",                     LUA_TFUNCTION,      {luavgl_anim_create}                 },
    {"Anim",                     LUA_TFUNCTION,      {luavgl_anim_create}                 },
    {"remove_all_anim",          LUA_TFUNCTION,      {luavgl_obj_remove_anim_all}         },
    {"__property",               LUA_TLIGHTUSERDATA, {.ptr = &property_table}             },
    {0,                          0,                  {0}                                  },
};

static void luavgl_obj_init(lua_State *L)
{
  luaL_newmetatable(L, "widgets");
  luaL_setfuncs(L, widget_create_methods, 0);
  lua_setfield(L, -1, "__index");

  /* base lv_obj */
  luavgl_obj_newmetatable(L, &lv_obj_class, "lv_obj", luavgl_obj_methods);
  lua_pushcfunction(L, luavgl_obj_gc);
  lua_setfield(L, -2, "__gc");

  lua_pop(L, 1); /* remove obj metatable */
}

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
LUALIB_API int luavgl_obj_set_property_kv(lua_State *L, void *data)
{
  struct setprop_para *para = data;
  lv_obj_t *obj = para->obj;

  /* Check for integer key with userdata as value */
  if (lua_type(L, -2) == LUA_TNUMBER && lua_type(L, -1) == LUA_TUSERDATA) {
    lv_obj_t *child = luavgl_to_obj(L, -1);
    lv_obj_set_parent(child, obj);
    return 0;
  }

  return obj_set_property_stack(L, obj, para->table);
}

LUALIB_API int luavgl_obj_create_helper(lua_State *L,
                                        lv_obj_t *(*create)(lv_obj_t *parent))
{
  luavgl_ctx_t *ctx = luavgl_context(L);
  lv_obj_t *parent;

  int type = lua_type(L, 1);
  if (type == LUA_TTABLE || type == LUA_TNONE || type == LUA_TNIL) {
    parent = ctx->root;
  } else {
    parent = luavgl_to_obj(L, 1);
    /* remove parent, in order to keep clean stack to call obj.set */
    lua_remove(L, 1);
  }

  LV_LOG_INFO("create obj on: %p", parent);

  lv_obj_t *obj = create(parent);
  luavgl_add_lobj(L, obj)->lua_created = true;

  if (!lua_istable(L, -2)) {
    /* no need to call setup */
    return 1;
  }

  int t = lua_getfield(L, -1, "__property"); /* obj on stack top */
  if (t != LUA_TLIGHTUSERDATA) {
    return luaL_error(L, "obj %p has no property table\n", obj);
  }

  const luavgl_table_t *properties = lua_touserdata(L, -1);
  lua_pop(L, 1); /* Get rid of the property userdata from stack */

  lua_insert(L, -2); /* obj, prop */

  /* now call obj.set to setup property */
  lua_getfield(L, -2, "set"); /* obj, prop, set */
  if (!luavgl_is_callable(L, -1)) {
    /* set method not found, call basic obj set method. */
    lua_pop(L, 2); /* remove prop table, and set method */
    luavgl_setup_obj(L, obj, properties);
  } else {
    lua_insert(L, -3); /* set, obj, prop */
    lua_call(L, 2, 0); /* now stack is clean */
    lua_pushlightuserdata(L, obj);
    lua_rawget(L, LUA_REGISTRYINDEX);
  }

  LV_LOG_INFO("create obj: %p", obj);
  return 1;
}

/**
 * Add existing lvgl obj to lua, return lobj(luavgl obj).
 * If no metatable not found for this obj class, then lv_obj_class metatable is
 * used
 */
LUALIB_API luavgl_obj_t *luavgl_add_lobj(lua_State *L, lv_obj_t *obj)
{
  luavgl_obj_t *lobj;

  /* In rare case, obj may be deleted but not gc'ed in lua, and lvgl quickly
   * creates another obj but happen to malloced same address, thus using obj
   * light userdata as key may have some potential issues. */
  lua_pushlightuserdata(L, obj);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (!lua_isnoneornil(L, -1)) {
    /* already exists */
    return luavgl_to_lobj(L, -1);
  }
  lua_pop(L, 1); /* pop nil value */

  lobj = lua_newuserdata(L, sizeof(*lobj));

  if (luavgl_obj_getmetatable(L, obj->class_p) == LUA_TNIL) {
    lua_pop(L, 1);
    LV_LOG_WARN("cannot find metatable for class: %p, %s", obj->class_p, obj->class_p->name);
    /* use base obj metatable instead */
    luavgl_obj_getmetatable(L, &lv_obj_class);
  }

  lua_setmetatable(L, -2);

  lv_memset(lobj, 0, sizeof(*lobj));
  lobj->obj = obj;

  /* Init event array to store events added from lua. */
  lv_array_init(&lobj->events, 0, sizeof(struct event_callback_s *));
  lv_obj_add_event_cb(obj, obj_delete_cb, LV_EVENT_DELETE, L);

  /* registry[obj] = lobj */
  lua_pushlightuserdata(L, obj);
  lua_pushvalue(L, -2);
  lua_rawset(L, LUA_REGISTRYINDEX);
  return lobj;
}
