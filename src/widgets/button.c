#include "luavgl.h"
#include "private.h"
#include "rotable.h"

static int luavgl_button_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_button_create);
}

static const rotable_Reg luavgl_button_methods[] = {
    {0, 0, {0}},
};

static void luavgl_button_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_button_class, "lv_button",
                          luavgl_button_methods);
  lua_pop(L, 1);
}
