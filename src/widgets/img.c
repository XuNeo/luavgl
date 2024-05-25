#include "luavgl.h"
#include "private.h"

static int luavgl_img_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_image_create);
}

static void lv_image_set_pivot_(void *obj, lua_State *L)
{
  if (!lua_istable(L, -1)) {
    luaL_argerror(L, -1, "should be table.");
    LV_LOG_ERROR("para should be table.");
    return;
  }

  lua_getfield(L, -1, "x");
  lv_coord_t x = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, -1, "y");
  lv_coord_t y = lua_tointeger(L, -1);
  lua_pop(L, 1);

  lv_image_set_pivot(obj, x, y);
}

static const luavgl_value_setter_t img_property_table[] = {
    {"src",
     SETTER_TYPE_IMGSRC,             {.setter_pointer = (setter_pointer_t)lv_image_set_src}},
    {"offset_x",  0,                 {.setter = (setter_int_t)lv_image_set_offset_x}       },
    {"offset_y",  0,                 {.setter = (setter_int_t)lv_image_set_offset_y}       },
    {"angle",     0,                 {.setter = (setter_int_t)lv_img_set_angle}          },
    {"zoom",      0,                 {.setter = (setter_int_t)lv_img_set_zoom}           },
    {"antialias", 0,                 {.setter = (setter_int_t)lv_image_set_antialias}      },
    {"pivot",     SETTER_TYPE_STACK, {.setter_stack = lv_image_set_pivot_}                 },
};

LUALIB_API int luavgl_img_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = luavgl_set_property(L, obj, img_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = luavgl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    LV_LOG_ERROR("unkown property for image");
  }

  return ret;
}

static int luavgl_img_set(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  /* lvgl requires img src should be set firstly */
  lua_getfield(L, -1, "src");
  if (!lua_isnoneornil(L, -1)) {
    const char *src = NULL;
    if (lua_isuserdata(L, -1)) {
      src = lua_touserdata(L, -1);
      LV_LOG_INFO("set img src to user data: %p", src);
    } else {
      src = lua_tostring(L, -1);
    }
    lv_image_set_src(obj, src);
  }
  lua_pop(L, 1);

  luavgl_iterate(L, -1, luavgl_img_set_property_kv, obj);

  return 0;
}

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

static const luaL_Reg luavgl_img_methods[] = {
    {"set",          luavgl_img_set       },

    {"set_src",      luavgl_img_set_src   },
    {"set_offset",   luavgl_img_set_offset},
    {"set_pivot",    luavgl_img_set_pivot },
    {"get_img_size", luavgl_get_img_size  },

    {NULL,           NULL                 },
};

static void luavgl_img_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_image_class, "lv_img", luavgl_img_methods);
  lua_pop(L, 1);
}
