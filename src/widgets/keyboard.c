#include "luavgl.h"
#include "private.h"
#include "rotable.h"

static int luavgl_keyboard_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_keyboard_create);
}

static const luavgl_table_t keyboard_property_table = {
    .len = 0,
    .array = NULL,
};

static const rotable_Reg luavgl_keyboard_methods[] = {
    {"__property", LUA_TLIGHTUSERDATA, {.ptr = &img_property_table}},

    {0,            0,                  {0}                         },
};

static void luavgl_keyboard_init(lua_State *L)
{
  static const rotable_Reg btm_methods[] = {
      {0, 0, {0}},
  };
  luavgl_obj_newmetatable(L, &lv_buttonmatrix_class, "lv_btnm", btm_methods);
  lua_pop(L, 1);

  luavgl_obj_newmetatable(L, &lv_keyboard_class, "lv_keyboard",
                          luavgl_keyboard_methods);
  lua_pop(L, 1);
}
