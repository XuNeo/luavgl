#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include "lugl.h"
#include "private.h"

static int lugl_calendar_create(lua_State *L)
{
  return lugl_obj_create_helper(L, lv_calendar_create);
}

static void _lv_calendar_set_today(void *obj, lua_State *L)
{
  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "expect date table.");
    return;
  }

  uint32_t year, month, day;
  lua_getfield(L, -1, "year");
  year = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "month");
  month = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "day");
  year = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lv_calendar_set_today_date(obj, year, month, day);
}

static void _lv_calendar_set_showed(void *obj, lua_State *L)
{
  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "expect date table.");
    return;
  }

  uint32_t year, month, day;
  lua_getfield(L, -1, "year");
  year = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "month");
  month = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lv_calendar_set_showed_date(obj, year, month);
}

static const lugl_value_setter_t calendar_property_table[] = {
    {"today", SETTER_TYPE_STACK, {.setter_stack = _lv_calendar_set_today}},
    {"showed", SETTER_TYPE_STACK, {.setter_stack = _lv_calendar_set_showed}},
};

static int lugl_calendar_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = lugl_set_property(L, obj, calendar_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = lugl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    debug("unkown property for calendar.\n");
  }

  return -1;
}

static int lugl_calendar_set(lua_State *L)
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

  lugl_iterate(L, -1, lugl_calendar_set_property_kv, obj);

  return 0;
}

static inline int calendar_new_date(lua_State *L,
                                    const lv_calendar_date_t *date)
{
  lua_createtable(L, 0, 3);
  lua_pushinteger(L, date->year);
  lua_setfield(L, -2, "year");

  lua_pushinteger(L, date->month);
  lua_setfield(L, -2, "month");

  lua_pushinteger(L, date->day);
  lua_setfield(L, -2, "day");

  return 1;
}

static int lugl_calendar_get_today(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  const lv_calendar_date_t *date = lv_calendar_get_today_date(obj);

  return calendar_new_date(L, date);
}

static int lugl_calendar_get_showed(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);
  const lv_calendar_date_t *date = lv_calendar_get_showed_date(obj);

  return calendar_new_date(L, date);
}

static int lugl_calendar_get_pressed(lua_State *L)
{
  lv_calendar_date_t date;
  lv_obj_t *obj = lugl_check_obj(L, 1);
  lv_calendar_get_pressed_date(obj, &date);

  return calendar_new_date(L, &date);
}

static int lugl_calendar_get_btnm(lua_State *L)
{
  lv_calendar_date_t date;
  lv_obj_t *obj = lugl_check_obj(L, 1);
  lv_obj_t *btm = lv_calendar_get_btnmatrix(obj);
  lua_pushlightuserdata(L, btm);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (lua_isnoneornil(L, -1)) {
    lugl_new_obj(L, btm);
  }

  return 1; /* obj userdata is already on stack */
}

static int lugl_calendar_create_arrow(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);

  return lugl_obj_create_helper(L, lv_calendar_header_arrow_create);
}

static int lugl_calendar_create_dropdown(lua_State *L)
{
  lv_obj_t *obj = lugl_check_obj(L, 1);

  return lugl_obj_create_helper(L, lv_calendar_header_dropdown_create);
}

static const luaL_Reg lugl_calendar_methods[] = {
    // calendar.c
    {"set", lugl_calendar_set},
    {"get_today", lugl_calendar_get_today},
    {"get_showed", lugl_calendar_get_showed},
    {"get_pressed", lugl_calendar_get_pressed},
    {"get_btnm", lugl_calendar_get_btnm},
    {"Arrow", lugl_calendar_create_arrow},
    {"Dropdown", lugl_calendar_create_dropdown},

    {NULL, NULL},
};

static void lugl_calendar_init(lua_State *L)
{
  luaL_newmetatable(L, "lv_calendar");

  lugl_new_objlib(L);
  luaL_setfuncs(L, lugl_calendar_methods, 0);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop __index table */
}
