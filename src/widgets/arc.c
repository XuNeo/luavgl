#include "luavgl.h"
#include "private.h"
#include "rotable.h"

static int luavgl_arc_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_arc_create);
}

static int arc_set_start_angle(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set) {
    lua_pushnumber(L, lv_arc_get_angle_start(obj));
    return 1;
  }
  lv_value_precise_t start_angle = lua_tonumber(L, -1);
  lv_arc_set_start_angle(obj, start_angle);
  return 1;
}

static int arc_set_end_angle(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set) {
    lua_pushnumber(L, lv_arc_get_angle_end(obj));
    return 1;
  }
  lv_value_precise_t end_angle = lua_tonumber(L, -1);
  lv_arc_set_end_angle(obj, end_angle);
  return 1;
}

static int arc_set_bg_start_angle(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set) {
    lua_pushnumber(L, lv_arc_get_angle_start(obj));
    return 1;
  }
  lv_value_precise_t start_angle = lua_tonumber(L, -1);
  lv_arc_set_bg_start_angle(obj, start_angle);
  return 1;
}

static int arc_set_bg_end_angle(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set) {
    lua_pushnumber(L, lv_arc_get_angle_end(obj));
    return 1;
  }
  lv_value_precise_t end_angle = lua_tonumber(L, -1);
  lv_arc_set_bg_end_angle(obj, end_angle);
  return 1;
}

static int arc_set_rotation(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set) {
    lua_pushinteger(L, lv_arc_get_rotation(obj));
    return 1;
  }
  int32_t rotation = lua_tointeger(L, -1);
  lv_arc_set_rotation(obj, rotation);
  return 1;
}

static int arc_set_value(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set) {
    lua_pushinteger(L, lv_arc_get_value(obj));
    return 1;
  }
  int32_t value = lua_tointeger(L, -1);
  lv_arc_set_value(obj, value);
  return 1;
}

static int arc_set_min_value(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set) {
    lua_pushinteger(L, lv_arc_get_min_value(obj));
    return 1;
  }
  int32_t min_value = lua_tointeger(L, -1);
  lv_arc_set_min_value(obj, min_value);
  return 1;
}

static int arc_set_max_value(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set) {
    lua_pushinteger(L, lv_arc_get_max_value(obj));
    return 1;
  }
  int32_t max_value = lua_tointeger(L, -1);
  lv_arc_set_max_value(obj, max_value);
  return 1;
}

static int arc_set_knob_offset(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set) {
    lua_pushinteger(L, lv_arc_get_knob_offset(obj));
    return 1;
  }
  int32_t knob_offset = lua_tointeger(L, -1);
  lv_arc_set_knob_offset(obj, knob_offset);
  return 1;
}

static int arc_set_mode(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set) {
    lua_pushinteger(L, lv_arc_get_mode(obj));
    return 1;
  }
  int32_t mode = lua_tointeger(L, -1);
  lv_arc_set_mode(obj, mode);
  return 1;
}

static const luavgl_property_ops_t arc_property_ops[] = {
    {.name = "start_angle",    .ops = arc_set_start_angle   },
    {.name = "end_angle",      .ops = arc_set_end_angle     },
    {.name = "bg_start_angle", .ops = arc_set_bg_start_angle},
    {.name = "bg_end_angle",   .ops = arc_set_bg_end_angle  },
    {.name = "rotation",       .ops = arc_set_rotation      },
    {.name = "value",          .ops = arc_set_value         },
    {.name = "min_value",      .ops = arc_set_min_value     },
    {.name = "max_value",      .ops = arc_set_max_value     },
    {.name = "knob_offset",    .ops = arc_set_knob_offset   },
    {.name = "mode",           .ops = arc_set_mode          },
};

static const luavgl_table_t arc_property_table = {
    .len = sizeof(arc_property_ops) / sizeof(arc_property_ops[0]),
    .array = arc_property_ops,
};

static const rotable_Reg luavgl_arc_methods[] = {
    {"__property", LUA_TLIGHTUSERDATA, {.ptr = &arc_property_table}},
    {},
};

static void luavgl_arc_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_arc_class, "lv_arc", luavgl_arc_methods);
  lua_pop(L, 1);
}
