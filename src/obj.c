#include "luavgl.h"
#include "private.h"

/* extended feature for object */
#include "event.c"
#include "rotable.h"
#include "style.c"
#include "widgets/widgets.c"

#if LV_USE_SNAPSHOT
#include "snapshot.c"
#endif

static int luavgl_anim_create(lua_State *L);
static int luavgl_obj_delete(lua_State *L);
static int luavgl_obj_clean(lua_State *L);

static const int obj_meta_key;

static void obj_delete_cb(lv_event_t *e)
{
  lua_State *L = e->user_data;
  lua_pushlightuserdata(L, e->current_target);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (lua_isnoneornil(L, -1)) {
    goto pop_exit;
  }

#if (LUA_VERSION_NUM >= 502)
  lua_pushnil(L);
  lua_setuservalue(L, -2);
#else
  lua_pushglobaltable(L);
  lua_setuservalue(L, -2);
#endif

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
    return luaL_error(L, "expect a table on 2nd para");
  }

  /* Iterate the table and set property */
  luavgl_iterate(L, -1, luavgl_obj_set_property_kv, obj);

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

static int luavgl_obj_get_child_by_id(lua_State *L)
{
  lv_obj_t *obj;
  const char *id;
  if (lua_type(L, 1) == LUA_TSTRING) {
    obj = NULL;
    id = lua_tostring(L, 1);
  } else {
    obj = luavgl_to_obj(L, 1);
    id = luaL_checkstring(L, 2);
  }

  lv_obj_t *child = lv_obj_get_child_by_id(obj, id);
  if (child == NULL) {
    lua_pushnil(L);
    return 1;
  }

  lua_pushlightuserdata(L, child);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (lua_isnil(L, -1)) {
    luavgl_add_lobj(L, child)->lua_created = false;
  }

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

/**
 * Use lvgl property API to get or set property.
 * stack layout:
 * 1: obj,
 * -2 property name, -1: property value if set is true
 * Return negative value if not found.
 */
static int obj_property_lvgl(lua_State *L, lv_obj_t *obj, lv_prop_id_t id,
                             bool set, const char *name)
{
  lv_property_t prop;
  /* A valid lvgl property, it must success. */
  if (set) {
    prop = luavgl_toproperty(L, -1, id);
    lv_result_t res = lv_obj_set_property(obj, &prop);
    if (res == LV_RESULT_OK) {
      return 0;
    }
  } else {
    prop = lv_obj_get_property(obj, id);
    if (prop.id != LV_PROPERTY_ID_INVALID) {
      luavgl_pushproperty(L, &prop);
      return 1;
    }
  }

  return luaL_error(L, "%s property failed: %s", set ? "set" : "get", name);
}

/**
 * Keep stack unchanged, get the ops for specified property name
 * stack layout:
 * 1: obj,
 * -2 property name, -1: property value if set is true
 * Return negative value if not found.
 */
int obj_property(lua_State *L, lv_obj_t *obj)
{
  /* -2 must be property name. */
  luaL_argcheck(L, lua_type(L, -2) == LUA_TSTRING, -2,
                "property name must be string");

  bool set = lua_type(L, -1) != LUA_TNIL;
  int top = lua_gettop(L);
  const char *name = luaL_checkstring(L, -2);
  const lv_obj_class_t *clz = obj->class_p;
  lv_prop_id_t id = LV_PROPERTY_ID_INVALID;

  for (; clz; clz = clz->base_class) {
    int t = luavgl_obj_getmetatable(L, clz);
    /* Does this class have metatable? */
    if (t == LUA_TTABLE) {
      /* If it has metatable, it must have field __index, which is a rotable */
      t = lua_getfield(L, -1, "__index");
      t = lua_getfield(L, -1, "__property");
      /* Does it provides __property ops table, which must be a ligthuserdata */
      if (t == LUA_TLIGHTUSERDATA) {
        const luavgl_table_t *table = lua_touserdata(L, -1);
        const luavgl_property_ops_t *ops = table->array;
        /* @todo, use bsearch and force property table in order. */
        for (int i = 0; i < table->len; i++) {
          if (lv_strcmp(ops[i].name, name) == 0) {
            lua_settop(L, top);
            /* Stack: 1, obj, -2, property name, -1, value if set is true */
            return ops[i].ops(L, obj, set);
          }
        }
      }
    }

    /* Restore the original stack. */
    lua_settop(L, top);

    /* Try lvgl base class */
    id = lv_obj_class_property_get_id(clz, name);
    if (id != LV_PROPERTY_ID_INVALID) {
      return obj_property_lvgl(L, obj, id, set, name);
    }
  }

  /* Try style, only set method supported. */
  if (set && luavgl_obj_set_style_kv(L, obj, LV_STATE_DEFAULT) == 0) {
    return 0;
  }

  /* Try lvgl style property */
  id = lv_style_property_get_id(name);
  if (id != LV_PROPERTY_ID_INVALID) {
    return obj_property_lvgl(L, obj, id, set, name);
  }

  return -1;
}

LUALIB_API int luavgl_obj_get_property(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushnil(L); /* 1: obj, -2: property name, -1: value(nil for get method) */
  return obj_property(L, obj);
}

#if LV_USE_OBJ_ID_BUILTIN == 0
/* For luavgl, we only support string as ID. */
void lv_obj_assign_id(const lv_obj_class_t *class_p, lv_obj_t *obj) {}

void lv_obj_free_id(lv_obj_t *obj)
{
  if (obj->id) {
    lv_free(obj->id);
    obj->id = NULL;
  }
}

void lv_obj_set_id(lv_obj_t *obj, void *id)
{
  lv_obj_free_id(obj);
  if (id != NULL) {
    obj->id = lv_strdup(id);
  }
}

const char *lv_obj_stringify_id(lv_obj_t *obj, char *buf, uint32_t len)
{
  if (obj->id == NULL)
    return NULL;
  lv_strlcpy(buf, (const char *)obj->id, len);
  return buf;
}

int lv_obj_id_compare(const void *id1, const void *id2)
{
  if (id1 == NULL || id2 == NULL)
    return -1;
  return lv_strcmp(id1, id2);
}

#endif

static int obj_property_id(lua_State *L, lv_obj_t *obj, bool set)
{
  if (set) {
    if (!lua_isstring(L, -1)) {
      return luaL_error(L, "id should be string");
    }

    lv_obj_set_id(obj, (void *)lua_tostring(L, -1));
    return 0;
  } else {
    /* get property */
    void *id = lv_obj_get_id(obj);
    id == NULL ? lua_pushnil(L) : lua_pushstring(L, id);
    return 1;
  }
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
    lua_pushinteger(L, lv_obj_get_style_align(obj, 0));
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

static int obj_property_user_data(lua_State *L, lv_obj_t *obj, bool set)
{
#if (LUA_VERSION_NUM >= 502)
  if (set) {
    lua_pushvalue(L, -1);
    lua_setuservalue(L, 1);
  } else {
    lua_getuservalue(L, 1);
  }
  return 1;
#else
  if (set) {
    lua_getuservalue(L, 1);
    lua_pushvalue(L, -1);
    lua_rawseti(L, -2, 1);
  } else {
    lua_getuservalue(L, 1);
    lua_rawgeti(L, -1, 1);
  }
  return 1;
#endif
}

static const luavgl_property_ops_t obj_property_ops[] = {
    {.name = "align",     .ops = obj_property_align    },
    {.name = "id",        .ops = obj_property_id       },
    {.name = "h",         .ops = obj_property_h        },
    {.name = "w",         .ops = obj_property_w        },
    {.name = "user_data", .ops = obj_property_user_data},
};

static const luavgl_table_t luavgl_obj_property_table = {
    .len = sizeof(obj_property_ops) / sizeof(obj_property_ops[0]),
    .array = obj_property_ops,
};

static const rotable_Reg luavgl_obj_methods[] = {
    {"set",                      LUA_TFUNCTION,      {luavgl_obj_set}                     },
    {"get",                      LUA_TFUNCTION,      {luavgl_obj_get_property}            },

    {"set_style",                LUA_TFUNCTION,      {luavgl_obj_set_style}               },
    {"align_to",                 LUA_TFUNCTION,      {luavgl_obj_align_to}                },
    {"delete",                   LUA_TFUNCTION,      {luavgl_obj_delete}                  },
    {"clean",                    LUA_TFUNCTION,      {luavgl_obj_clean}                   },
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
    {"get_child_by_id",          LUA_TFUNCTION,      {luavgl_obj_get_child_by_id}         },

    {"onevent",                  LUA_TFUNCTION,      {luavgl_obj_on_event}                },
    {"onPressed",                LUA_TFUNCTION,      {luavgl_obj_on_pressed}              },
    {"onClicked",                LUA_TFUNCTION,      {luavgl_obj_on_clicked}              },
    {"onShortClicked",           LUA_TFUNCTION,      {luavgl_obj_on_short_clicked}        },
    {"anim",                     LUA_TFUNCTION,      {luavgl_anim_create}                 },
    {"Anim",                     LUA_TFUNCTION,      {luavgl_anim_create}                 },
    {"remove_all_anim",          LUA_TFUNCTION,      {luavgl_obj_remove_anim_all}         },
    {"__property",               LUA_TLIGHTUSERDATA, {.ptr = &luavgl_obj_property_table}  },
#if LV_USE_SNAPSHOT
    {"snapshot",                 LUA_TFUNCTION,      {luavgl_obj_snapshot}                },
#endif
    {0,                          0,                  {0}                                  },
};

/**
 * On stack: 1: obj, 2: key to get
 */
static int obj_meta__index(lua_State *L)
{
  int t;
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  const char *key = lua_tostring(L, 2);

  t = luavgl_obj_getmetatable(L, obj->class_p);
  if (t == LUA_TNIL || t == LUA_TNONE) {
    lua_pop(L, 1);
    /* This class has no metatable, use base class instead. */
    t = luavgl_obj_getmetatable(L, &lv_obj_class);
  }

  lua_getfield(L, -1, "__index");

  /* Try to get from table directly. */
  lua_pushvalue(L, 2);
  t = lua_gettable(L, -2);
  if (t != LUA_TNIL) {
    return 1;
  }
  lua_pop(L, 1); /* Get rid of the nil value */

  /* push key to stack */
  lua_pushvalue(L, 2);
  lua_pushnil(L); /* Property value is nil for get method. */

  /* Try property */
  int ret = obj_property(L, obj);
  if (ret >= 0) {
    return ret;
  }

  return luaL_error(L, "property not found: %s", key);
}

/**
 * On stack: 1: obj, 2: key to set, 3: value
 */
static int obj_meta__newindex(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  return obj_property(L, obj);
}

static int obj_meta__tostring(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushfstring(L, "%s(%p)", obj->class_p->name, obj);
  return 1;
}

static int obj_meta__gc(lua_State *L)
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

static const luaL_Reg obj_meta_methods[] = {
    {"__index",    obj_meta__index   },
    {"__newindex", obj_meta__newindex},
    {"__tostring", obj_meta__tostring},
    {"__gc",       obj_meta__gc      },
    {NULL,         NULL              }
};

static void luavgl_obj_init(lua_State *L)
{
  /* 1. The metatable for all lv_obj_t userdata. */
  luavgl_createmetatable(L, &obj_meta_key, "luavglObj");
  luaL_setfuncs(L, obj_meta_methods, 0);
  lua_pop(L, 1);

  /* 2. Widget creation methods table */
  luaL_newmetatable(L, "widgets");
  luaL_setfuncs(L, widget_create_methods, 0);
  lua_setfield(L, -1, "__index");

  /* 3. Base lv_obj metatable */
  luavgl_obj_newmetatable(L, &lv_obj_class, "lv_obj", luavgl_obj_methods);
  lua_pop(L, 1); /* remove obj metatable */

  /* Register to global function */
  lua_pushcfunction(L, luavgl_obj_get_child_by_id);
  lua_setfield(L, -2, "get_child_by_id");
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
  lv_obj_t *obj = data;

  /* Check for integer key with userdata as value */
  if (lua_type(L, -2) == LUA_TNUMBER && lua_type(L, -1) == LUA_TUSERDATA) {
    lv_obj_t *child = luavgl_to_obj(L, -1);
    lv_obj_set_parent(child, obj);
    return 0;
  }

  return obj_property(L, obj);
}

/**
 * Create an object with parent and property table.
 * On stack:
 *     1. parent, 2. property table
 *  Or 1. property table
 */
LUALIB_API int luavgl_obj_create_helper(lua_State *L,
                                        lv_obj_t *(*create)(lv_obj_t *parent))
{
  luavgl_ctx_t *ctx = luavgl_context(L);
  lv_obj_t *parent;

  int type = lua_type(L, 1);
  if (type == LUA_TTABLE) {
    parent = ctx->root;
  } else if (type == LUA_TNONE || type == LUA_TNIL) {
    parent = ctx->root;
    lua_remove(L, 1); /* Remove nil from stack */
  } else {
    parent = luavgl_to_obj(L, 1);
    lua_remove(L, 1); /* Remove parent from stack */
  }

  lv_obj_t *obj = create(parent);
  luavgl_add_lobj(L, obj)->lua_created = true;
  if (!lua_istable(L, 1)) {
    /* no need to call setup */
    return 1;
  }

  int top = lua_gettop(L);

  /* Prepare a new stack for set property.  */
  lua_pushcfunction(L, luavgl_obj_set);
  lua_pushvalue(L, -2); /* obj */
  lua_pushvalue(L, 1);  /* prop table */
  lua_call(L, 2, 0);

  lua_settop(L, top); /* Keep only the obj */
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

  /* Add obj metatable to userdata. */
  if (luavgl_getmetatable(L, &obj_meta_key) == LUA_TNIL) {
    luaL_error(L, "obj.meta metatable not found");
    return NULL;
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

#if (LUA_VERSION_NUM == 501)
  lua_newtable(L);
  lua_setuservalue(L, -2);
#endif

  LV_LOG_INFO("add obj: %p to lua, lobj: %p", obj, lua_touserdata(L, -1));
  return lobj;
}
