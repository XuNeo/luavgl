#include "luavgl.h"
#include "private.h"

static int luavgl_textarea_create(lua_State *L)
{
  return luavgl_obj_create_helper(L, lv_textarea_create);
}

static const rotable_Reg luavgl_textarea_methods[] = {
    {0, 0, {0}},
};

static void luavgl_textarea_init(lua_State *L)
{
  luavgl_obj_newmetatable(L, &lv_textarea_class, "lv_textarea",
                          luavgl_textarea_methods);
  lua_pop(L, 1);
}
