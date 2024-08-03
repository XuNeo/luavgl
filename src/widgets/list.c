#include "luavgl.h"
#include "private.h"

static int luavgl_list_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_list_create);
}

/* clang-format off */
static const luavgl_value_setter_t list_property_table[] = {
    {"dummy", SETTER_TYPE_STACK, {.setter_stack = _lv_dummy_set}},
};
/* clang-format on */

static int luavgl_list_add_text(lua_State *L)
{
  lv_obj_t *list = luavgl_to_obj(L, 1);
  const char *str = lua_tostring(L, 2);
  lv_obj_t *obj = lv_list_add_text(list, str);
  luavgl_add_lobj(L, obj)->lua_created = true;
  lua_settop(L, 1);
  return 1;
}

static int luavgl_list_add_btn(lua_State *L)
{
  lv_obj_t *list = luavgl_to_obj(L, 1);
  const void *icon = luavgl_toimgsrc(L, 2);
  const char *str = lua_tostring(L, 3);
  lv_obj_t *obj = lv_list_add_btn(list, icon, str);
  luavgl_add_lobj(L, obj)->lua_created = true;
  return 1;
}

static int luavgl_get_btn_text(lua_State *L)
{
  lv_obj_t *list = luavgl_to_obj(L, 1);
  lv_obj_t *btn = luavgl_to_obj(L, 2);

  lua_pushstring(L, lv_list_get_btn_text(list, btn));
  return 1;
}

static const rotable_Reg luavgl_list_methods[] = {
    {"add_text",     LUA_TFUNCTION,      {luavgl_list_add_text}      },
    {"add_btn",      LUA_TFUNCTION,      {luavgl_list_add_btn}       },
    {"get_btn_text", LUA_TFUNCTION,      {luavgl_get_btn_text}       },

    {0,              0,                  {0}                         },
};

static void luavgl_list_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_list_class, "lv_list", luavgl_list_methods);
  lua_pop(L, 1);
}
