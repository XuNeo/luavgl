#ifndef WIDGETS_WIDGETS_H_
#define WIDGETS_WIDGETS_H_

#include <lua.h>

void luavgl_extension_init(lua_State *L);
void luavgl_pointer_init(lua_State *L);
void luavgl_analog_time_init(lua_State *L);

void luavgl_widgets_init(lua_State *L);

#endif /* WIDGETS_WIDGETS_H_ */
