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

#include <lua.h>
#include <lvgl.h>
#include "rotable.h"


/* The lookup code uses binary search on sorted `rotable_Reg` arrays
 * to find functions/methods. For a small number of elements a linear
 * search might be faster. */
#ifndef ROTABLE_BINSEARCH_MIN
#  define ROTABLE_BINSEARCH_MIN  5
#endif


typedef struct {
  /* We use store it in the userdata payload. */
  rotable_Reg const* p;
  /* number of elements in the luaL_Reg array *if* it is sorted by
   * name. We can use binary search in this case. Otherwise we need
   * linear searches anyway and we can scan for the final `{NULL,0}`
   * entry. `n` is 0 in this case. */
  int n;
} rotable;


static char const unique_address[ 1 ] = { 0 };


static int reg_compare(void const* a, void const* b) {
  return lv_strcmp( (char const*)a, ((rotable_Reg const*)b)->name );
}


static rotable* check_rotable( lua_State* L, int idx, char const* func ) {
  rotable* t = (rotable*)lua_touserdata( L, idx );
  if( t ) {
    if( lua_getmetatable( L, idx ) ) {
      lua_pushlightuserdata( L, (void*)unique_address );
      lua_rawget( L, LUA_REGISTRYINDEX );
      if( !lua_rawequal( L, -1, -2 ) )
        t = 0;
      lua_pop( L, 2 );
    }
  }
  if( !t ) {
    char const* type = lua_typename( L, lua_type( L, idx ) );
    if( lua_type( L, idx ) == LUA_TLIGHTUSERDATA ) {
      type = "light userdata";
    } else if( lua_getmetatable( L, idx ) ) {
      lua_getfield( L, -1, "__name" );
      lua_replace( L, -2 ); /* we don't need the metatable anymore */
      if( lua_type( L, -1 ) == LUA_TSTRING )
        type = lua_tostring( L, -1 );
    }
    lua_pushfstring( L, "bad argument #%d to '%s' "
                     "(rotable expected, got %s)", idx, func, type );
    lua_error( L );
  }
  return t;
}


static rotable_Reg const* find_key( rotable_Reg const* p, int n,
                                    char const* s ) {
  if( s ) {
    if( n >= ROTABLE_BINSEARCH_MIN ) { /* binary search */
      return (rotable_Reg const*)lv_utils_bsearch( s, p, n, sizeof( *p ), reg_compare );
    } else { /* use linear scan */
      for( ; p->name; ++p ) {
        if( 0 == reg_compare( s, p ) )
          return p;
      }
    }
  }
  return 0;
}

static void rotable_pushvalue( lua_State* L, rotable_Reg const* p ) {
  switch( p->type ) {
    case LUA_TFUNCTION:
      lua_pushcfunction( L, p->func );
      break;
    case LUA_TSTRING:
      lua_pushstring( L, p->str );
      break;
    case LUA_TLIGHTUSERDATA:
      lua_pushlightuserdata( L, (void*)p->ptr );
      break;
    case LUA_TNUMBER:
      lua_pushnumber( L, p->number );
      break;
    default:
      lua_pushinteger( L, p->integer );
      break;
  }
}

static int rotable_func_index( lua_State* L ) {
  char const* s = lua_tostring( L, 2 );
  rotable_Reg const* p = (rotable_Reg const*)lua_touserdata( L, lua_upvalueindex( 1 ) );
  int n = lua_tointeger( L, lua_upvalueindex( 2 ) );
  p = find_key( p, n, s );
  if( p )
    rotable_pushvalue( L, p );
  else
    lua_pushnil( L );
  return 1;
}


static int rotable_udata_index( lua_State* L ) {
  rotable* t = (rotable*)lua_touserdata( L, 1 );
  char const* s = lua_tostring( L, 2 );
  rotable_Reg const* p = t->p;
  p = find_key( p, t->n, s );
  if( p )
    rotable_pushvalue( L, p );
  else {
    int type = lua_getuservalue( L, 1 );
    if( type == LUA_TTABLE ) {
      type = lua_getfield( L, -1, "__index" );
      if( type == LUA_TTABLE || type == LUA_TUSERDATA ) {
        lua_pushvalue( L, 2 );
        lua_gettable( L, -2 );
        return 1;
      }
      else if( type == LUA_TFUNCTION ){
        lua_pushvalue( L, -2 );
        lua_pushvalue( L, 2 );
        lua_call( L, 2, 1 );
        return 1;
      }
    }

    /* Otherwise, not found.*/
    lua_pushnil( L );
  }
  return 1;
}


static int rotable_udata_len( lua_State* L ) {
  lua_pushinteger( L, 0 );
  return 1;
}


static int rotable_iter( lua_State* L ) {
  rotable* t = check_rotable( L, 1, "__pairs iterator" );
  char const* s = lua_tostring( L, 2 );
  rotable_Reg const* q = 0;
  rotable_Reg const* p = t->p;
  if( s ) {
    if( t->n >= ROTABLE_BINSEARCH_MIN ) { /* binary search */
      q = (rotable_Reg const*)lv_utils_bsearch( s, p, t->n, sizeof( *p ), reg_compare );
      if( q )
        ++q;
      else
        q = p + t->n;
    } else { /* use linear scan */
      for( q = p; q->name; ++q ) {
        if( 0 == reg_compare( s, q ) ) {
          ++q;
          break;
        }
      }
    }
  } else
    q = p;

  lua_pushstring( L, q->name );
  rotable_pushvalue( L, q );
  return 2;
}


static int rotable_udata_pairs( lua_State* L ) {
  lua_pushcfunction( L, rotable_iter );
  lua_pushvalue( L, 1 );
  lua_pushnil( L );
  return 3;
}


ROTABLE_EXPORT void rotable_newlib( lua_State* L, void const* v ) {
  rotable_Reg const* reg = (rotable_Reg const*)v;
  rotable* t = (rotable*)lua_newuserdata( L, sizeof( *t ) );
  lua_pushlightuserdata( L, (void*)unique_address );
  lua_rawget( L, LUA_REGISTRYINDEX );
  if( !lua_istable( L, -1 ) ) {
    lua_pop( L, 1 );
    lua_createtable( L, 0, 5 );
    lua_pushcfunction( L, rotable_udata_index );
    lua_setfield( L, -2, "__index" );
    lua_pushcfunction( L, rotable_udata_len );
    lua_setfield( L, -2, "__len" );
    lua_pushcfunction( L, rotable_udata_pairs );
    lua_setfield( L, -2, "__pairs" );
    lua_pushboolean( L, 0 );
    lua_setfield( L, -2, "__metatable" );
    lua_pushliteral( L, "rotable" );
    lua_setfield( L, -2, "__name" );
    lua_pushlightuserdata( L, (void*)unique_address );
    lua_pushvalue( L, -2 );
    lua_rawset( L, LUA_REGISTRYINDEX );
  }
  lua_setmetatable( L, -2 );
  t->p = reg;
  t->n = 0;
  if( reg->name ) {
    int i = 1;
    for( ; reg[ i ].name; ++i ) {
      if( lv_strcmp( reg[ i-1 ].name, reg[ i ].name ) >= 0 )
        return;
    }
    t->n = i;
  }
}

ROTABLE_EXPORT void rotable_setmetatable( lua_State* L, int idx) {
  /* Store a table to uservalue as rotable's "metatable" */
  lua_setuservalue( L, idx );
}

ROTABLE_EXPORT void rotable_newidx( lua_State* L, void const* v ) {
  rotable_Reg const* reg = (rotable_Reg const*)v;
  int i = 1;
  lua_pushlightuserdata( L, (void*)v);
  for( ; reg[ i ].name; ++i ) {
    if( lv_strcmp( reg[ i-1 ].name, reg[ i ].name ) >= 0 ) {
      i = 0;
      break;
    }
  }
  if( i >= ROTABLE_BINSEARCH_MIN ) {
    lua_pushinteger( L, i );
    lua_pushcclosure( L, rotable_func_index, 2 );
  } else
    lua_pushcclosure( L, rotable_func_index, 1 );
}