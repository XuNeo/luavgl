#include "luavgl.h"
#include "private.h"

static int luavgl_led_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_led_create);
}

/* clang-format off */
static const luavgl_value_setter_t led_property_table[] = {
    {"color", SETTER_TYPE_COLOR, {.setter = (setter_int_t)lv_led_set_color}},
    {"brightness", 0, {.setter = (setter_int_t)lv_led_set_brightness}},
};
/* clang-format on */

LUALIB_API int luavgl_led_set_property_kv(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = luavgl_set_property(L, obj, led_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = luavgl_obj_set_property_kv(L, obj);
  if (ret != 0) {
    debug("unkown property for led.\n");
  }

  return ret;
}

static int luavgl_led_set(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);

  if (!lua_istable(L, -1)) {
    luaL_error(L, "expect a table on 2nd para.");
    return 0;
  }

  luavgl_iterate(L, -1, luavgl_led_set_property_kv, obj);

  return 0;
}

static int luavgl_led_on(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_led_on(obj);
  return 0;
}

static int luavgl_led_off(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_led_off(obj);
  return 0;
}

static int luavgl_led_toggle(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lv_led_toggle(obj);
  return 0;
}

static int luavgl_led_get_brightness(lua_State *L)
{
  lv_obj_t *obj = luavgl_to_obj(L, 1);
  lua_pushinteger(L, lv_led_get_brightness(obj));
  return 1;
}

static const luaL_Reg luavgl_led_methods[] = {
    {"set",            luavgl_led_set           },
    {"on",             luavgl_led_on            },
    {"off",            luavgl_led_off           },
    {"toggle",         luavgl_led_toggle        },

    {"get_brightness", luavgl_led_get_brightness},

    {NULL,             NULL                   },
};

static void luavgl_led_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_led_class, "lv_led", luavgl_led_methods);
  lua_pop(L, 1);
}
