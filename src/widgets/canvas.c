#include "luavgl.h"
#include "private.h"

static int luavgl_canvas_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_canvas_create);
}

static void _lv_canvas_set_buffer(void *obj, lua_State *L)
{
  luaL_argcheck(L, lua_istable(L, -1), -1, "expect a table.");

  lua_getfield(L, -1, "w");
  int w = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "h");
  int h = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "cf");
  lv_img_cf_t cf = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lv_img_dsc_t *dsc = (lv_img_dsc_t *)lv_img_get_src(obj);
  if (dsc->data != NULL) {
    luaL_error(L, "canvas buffer already set.");
  }

  void *buf = lv_mem_alloc(lv_img_buf_get_img_size(w, h, cf));
  if (buf == NULL) {
    luaL_error(L, "No memory.");
  }

  lv_canvas_set_buffer(obj, buf, w, h, cf);
}

static void _lv_canvas_set_px(void *obj, lua_State *L)
{
  luaL_argcheck(L, lua_istable(L, -1), -1, "expect a table.");

  lua_getfield(L, -1, "x");
  int x = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "y");
  int y = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "color");
  lv_color_t color = luavgl_tocolor(L, -1);
  lua_pop(L, 1);

  lv_canvas_set_px(obj, x, y, color);
}

static void _lv_canvas_set_px_opa(void *obj, lua_State *L)
{
  luaL_argcheck(L, lua_istable(L, -1), -1, "expect a table.");

  lua_getfield(L, -1, "x");
  int x = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "y");
  int y = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "opa");
  lv_opa_t opa = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lv_canvas_set_px_opa(obj, x, y, opa);
}

static void _lv_canvas_set_palette(void *obj, lua_State *L)
{
  luaL_argcheck(L, lua_istable(L, 2), 2, "expect a table.");

  lua_getfield(L, 2, "id");
  int id = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, 2, "color");
  lv_color_t color = luavgl_tocolor(L, -1);
  lua_pop(L, 1);

  lv_canvas_set_palette(obj, id, color);
}

static const luavgl_value_setter_t canvas_property_table[] = {
    {"buffer",  SETTER_TYPE_STACK, {.setter_stack = _lv_canvas_set_buffer} },
    {"px",      SETTER_TYPE_STACK, {.setter_stack = _lv_canvas_set_px}     },
    {"px_opa",  SETTER_TYPE_STACK, {.setter_stack = _lv_canvas_set_px_opa} },
    {"palette", SETTER_TYPE_STACK, {.setter_stack = _lv_canvas_set_palette}},
};

LUALIB_API int luavgl_canvas_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = luavgl_set_property(L, obj, canvas_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = luavgl_img_set_property_kv(L, obj);
  if (ret != 0) {
    debug("unkown property for canvas.\n");
  }

  return ret;
}

static int luavgl_canvas_set(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  luavgl_iterate(L, -1, luavgl_canvas_set_property_kv, obj);

  return 0;
}

static int luavgl_canvas_get_px(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  int x = luaL_checkinteger(L, 2);
  int y = luaL_checkinteger(L, 3);

  lv_color_t c = lv_canvas_get_px(obj, x, y);
  lua_newtable(L);
  lua_pushinteger(L, c.ch.red);
  lua_setfield(L, -2, "r");
  lua_pushinteger(L, c.ch.green);
  lua_setfield(L, -2, "g");
  lua_pushinteger(L, c.ch.blue);
  lua_setfield(L, -2, "b");

  return 1;
}

static int luavgl_canvas_get_img(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_img_dsc_t *img = lv_canvas_get_img(obj);
  if (img == NULL) {
    lua_pushnil(L);
    return 1;
  }

  lua_pushlightuserdata(L, img);
  return 1;
}

static int luavgl_canvas_blur_hor(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_area_t area;

  if (!lua_istable(L, 2)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  lua_getfield(L, 2, "x1");
  area.x1 = lua_tointeger(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 2, "y1");
  area.y1 = lua_tointeger(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 2, "x2");
  area.x2 = lua_tointeger(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 2, "y2");
  area.y2 = lua_tointeger(L, -1);
  lua_pop(L, 1);

  uint16_t r = luaL_checkinteger(L, 3);
  lv_canvas_blur_hor(obj, &area, r);
  return 0;
}

static int luavgl_canvas_blur_ver(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_area_t area;

  if (!lua_istable(L, 2)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  lua_getfield(L, 2, "x1");
  area.x1 = lua_tointeger(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 2, "y1");
  area.y1 = lua_tointeger(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 2, "x2");
  area.x2 = lua_tointeger(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 2, "y2");
  area.y2 = lua_tointeger(L, -1);
  lua_pop(L, 1);

  uint16_t r = luaL_checkinteger(L, 3);
  lv_canvas_blur_ver(obj, &area, r);
  return 0;
}

static int luavgl_canvas_fill_bg(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_color_t color = luavgl_tocolor(L, 2);
  lv_opa_t opa = luaL_checkinteger(L, 3);
  lv_canvas_fill_bg(obj, color, opa);
  return 0;
}

static const luaL_Reg luavgl_canvas_methods[] = {
    {"set",      luavgl_canvas_set     },
    {"get_px",   luavgl_canvas_get_px  },
    {"get_img",  luavgl_canvas_get_img },
    {"blur_hor", luavgl_canvas_blur_hor},
    {"blur_ver", luavgl_canvas_blur_ver},
    {"fill_bg",  luavgl_canvas_fill_bg },

    {NULL,       NULL                  },
};

static void luavgl_canvas_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_canvas_class, "lv_canvas",
                          luavgl_canvas_methods);
  lua_pop(L, 1);
}
