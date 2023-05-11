#include "widgets.h"

void luavgl_widgets_init(lua_State *L)
{
  luavgl_extension_init(L);
  luavgl_pointer_init(L);
  luavgl_analog_time_init(L);
}