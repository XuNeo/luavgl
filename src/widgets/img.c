#include "luavgl.h"
#include "private.h"
#include "rotable.h"

static int luavgl_img_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_image_create);
}

static int image_set_pivot(lua_State *L, lv_obj_t *obj, bool set)
{
  if (!set) {
    /* Read pivot */
    lv_point_t p;
    lv_image_get_pivot(obj, &p);
    lua_createtable(L, 0, 2);
    lua_pushinteger(L, p.x);
    lua_setfield(L, -2, "x");
    lua_pushinteger(L, p.y);
    lua_setfield(L, -2, "y");
    return 1;
  }

  lv_point_t point = luavgl_topoint(L, -1);
  lv_image_set_pivot(obj, point.x, point.y);
  return 1;
}

static const luavgl_property_ops_t img_property_ops[] = {
    {.name = "pivot", .ops = image_set_pivot},
};

static const luavgl_table_t img_property_table = {
    .len = sizeof(img_property_ops) / sizeof(img_property_ops[0]),
    .array = img_property_ops,
};

/**
 * img.set_src(img, "path")
 */
static int luavgl_img_set_src(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  const char *src = luavgl_toimgsrc(L, 2);
  if (src != NULL) {
    lv_image_set_src(obj, src);
  }

  return 0;
}

/**
 * img:set_offset({x=10})
 * img:set_offset({x=10})
 * img:set_offset({x=10, y=100})
 */
static int luavgl_img_set_offset(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "should be table {x=0,y=0,anim=true}");
  }

  lv_coord_t v;
  lua_getfield(L, -1, "x");
  if (!lua_isnil(L, -1)) {
    v = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lv_image_set_offset_x(obj, v);
  }

  lua_getfield(L, -1, "y");
  if (!lua_isnil(L, -1)) {
    v = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lv_image_set_offset_y(obj, v);
  }

  return 0;
}

/**
 * img:set_pivot({x=10, y=100})
 */
static int luavgl_img_set_pivot(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "should be table {x=0,y=0,anim=true}");
  }

  lv_coord_t x = 0, y = 0;
  lua_getfield(L, -1, "x");
  x = lua_tointeger(L, -1);

  lua_getfield(L, -1, "y");
  y = lua_tointeger(L, -1);

  lv_image_set_pivot(obj, x, y);

  return 0;
}

/**
 * return image size w, h
 * img:get_img_size() -- get size of this image
 * img:get_img_size("src") -- get size of img "src"
 */
static int luavgl_get_img_size(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  const void *src = NULL;
  if (lua_isnoneornil(L, 2)) {
    src = lv_image_get_src(obj);
  } else {
    src = luavgl_toimgsrc(L, 2);
  }

  lv_image_header_t header;
  if (src == NULL || lv_image_decoder_get_info(src, &header) != LV_RES_OK) {
    lua_pushnil(L);
    lua_pushnil(L);
  } else {
    lua_pushinteger(L, header.w);
    lua_pushinteger(L, header.h);
  }

  return 2;
}

static const rotable_Reg luavgl_img_methods[] = {
    {"set_src",      LUA_TFUNCTION,      {luavgl_img_set_src}        },
    {"set_offset",   LUA_TFUNCTION,      {luavgl_img_set_offset}     },
    {"set_pivot",    LUA_TFUNCTION,      {luavgl_img_set_pivot}      },
    {"get_img_size", LUA_TFUNCTION,      {luavgl_get_img_size}       },
    {"__property",   LUA_TLIGHTUSERDATA, {.ptr = &img_property_table}},
    {0,              0,                  {0}                         },
};

static void luavgl_img_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_image_class, "lv_img", luavgl_img_methods);
  lua_pop(L, 1);
}
