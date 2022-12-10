#include <lauxlib.h>
#include <lua.h>

#include <lvgl.h>
#include <stdlib.h>

#include "lugl.h"
#include "private.h"

static int lugl_label_create(lua_State *L)
{
  return lugl_obj_create_helper(L, lv_label_create);
}

static void _lv_label_set_txt(void *obj, lua_State *L)
{
  if (!lua_isstring(L, -1)) {
    luaL_argerror(L, -1, "only support string for text.");
    return;
  }

  lv_label_set_text(obj, lua_tostring(L, -1));
}

static const lugl_value_setter_t label_property_table[] = {
    {"text", SETTER_TYPE_STACK, {.setter_stack = _lv_label_set_txt}},
};

static int label_set_property_cb(lua_State *L, void *data)
{
  lv_obj_t *obj = data;
  int ret = lugl_set_property(L, obj, label_property_table);

  if (ret == 0) {
    return 0;
  }

  /* a base obj property? */
  ret = obj_set_property_cb(L, obj);
  if (ret != 0) {
    debug("unkown property for label.\n");
  }

  return -1;
}

/**
 * set the property of object like x, y, w, h etc.
 * #1: obj: lightuserdata
 * #2: {k = v} key: string, choose from x, y, w, h, value: any
 */
static int lugl_label_set(lua_State *L)
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

  lugl_iterate(L, -1, label_set_property_cb, obj);

  return 0;
}
