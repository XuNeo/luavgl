/**
 * rotable (c) 2017 Philipp Janda
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDER BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef ROTABLE_H_
#define ROTABLE_H_

#include <lua.h>

/* exactly the same as luaL_Reg, but since we are on small embedded
 * microcontrollers, we don't assume that you have `lauxlib.h`
 * available in your build! */
typedef struct rotable_Reg {
  char const* name;
  lua_CFunction func;
} rotable_Reg;

#ifndef ROTABLE_EXPORT
#  define ROTABLE_EXPORT extern
#endif

/* compatible with `luaL_newlib()`, and works with `luaL_Reg` *and*
 * `rotable_Reg` arrays (in case you don't use `lauxlib.h`) */
ROTABLE_EXPORT void rotable_newlib( lua_State* L, void const* reg );

/* Since userdatas can not be used as `__index` meta methods directly
 * this function creates a C closure that looks up keys in a given
 * `rotable_Reg` array. */
ROTABLE_EXPORT void rotable_newidx( lua_State* L, void const* reg );

#endif /* ROTABLE_H_ */