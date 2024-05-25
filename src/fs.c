#include "luavgl.h"
#include "private.h"

typedef struct luavgl_fs_s {
  lv_fs_file_t file;
  bool closed; /* userdata exists but lv_fs has been closed */
} luavgl_fs_t;

typedef struct luavgl_dir_s {
  lv_fs_dir_t dir;
  bool closed; /* userdata exists but lv_fs has been closed */
} luavgl_dir_t;

static luavgl_fs_t *luavgl_to_fs(lua_State *L, int index)
{
  luavgl_fs_t *v = luaL_checkudata(L, index, "lv_fs");
  if (v->closed) {
    luaL_error(L, "attempt to use a closed file");
  }

  return v;
}

static luavgl_dir_t *luavgl_to_dir(lua_State *L, int index)
{
  luavgl_dir_t *v = luaL_checkudata(L, index, "lv_dir");
  if (v->closed) {
    luaL_error(L, "attempt to use a closed file");
  }

  return v;
}

/**
 * luavgl.open_file(filename, [mode])
 * mode: "r" "w", others not supported.
 */
static int luavgl_fs_open(lua_State *L)
{
  const char *path = lua_tostring(L, 1);
  const char *mode = "r";
  if (lua_type(L, 2) == LUA_TSTRING) {
    mode = lua_tostring(L, 2);
  }

  luavgl_fs_t *f = lua_newuserdata(L, sizeof(luavgl_fs_t));
  f->closed = false;

  lv_fs_mode_t lmode = 0;
  if (luavgl_strchr(mode, 'r'))
    lmode |= LV_FS_MODE_RD;

  if (luavgl_strchr(mode, 'w'))
    lmode |= LV_FS_MODE_WR;

  lv_fs_res_t res = lv_fs_open(&f->file, path, lmode);
  if (res != LV_FS_RES_OK) {
    LV_LOG_ERROR("open failed: %s", path);
    lua_pushnil(L);
    lua_pushfstring(L, "open failed: %s\n", path);
    lua_pushinteger(L, res); /* return lv_fs error number */
    return 3;
  }

  luaL_getmetatable(L, "lv_fs");
  lua_setmetatable(L, -2);

  return 1;
}

/* read_chars modified for lv_fs */
static int read_chars(lua_State *L, lv_fs_file_t *f, size_t n)
{
  uint32_t nr; /* number of chars actually read */
  char *p;

  luaL_Buffer b;
  luaL_buffinit(L, &b);
  p = luaL_prepbuffsize(&b, n); /* prepare buffer to read whole block */
  lv_fs_res_t res = lv_fs_read(f, p, n, &nr);
  if (res != LV_FS_RES_OK)
    return false;

  luaL_pushresultsize(&b, nr);
  return (nr > 0); /* true iff read something */
}

/* read_all modified for lv_fs */
static void read_all(lua_State *L, lv_fs_file_t *f)
{
  uint32_t nr;
  luaL_Buffer b;
  luaL_buffinit(L, &b);
  do { /* read file in chunks of LUAL_BUFFERSIZE bytes */
    char *p = luaL_prepbuffer(&b);
    lv_fs_read(f, p, LUAL_BUFFERSIZE, &nr);
    luaL_addsize(&b, nr);
  } while (nr == LUAL_BUFFERSIZE);
  luaL_pushresult(&b); /* close buffer */
}

/**
 * f:read()
 */
static int luavgl_fs_read(lua_State *L)
{
  luavgl_fs_t *f = luavgl_to_fs(L, 1);
  int nargs = lua_gettop(L) - 1;
  int n, success;

  if (nargs == 0)
    return luaL_error(L, "read mode required: 'a' or num ");

  /* ensure stack space for all results and for auxlib's buffer */
  luaL_checkstack(L, nargs + LUA_MINSTACK, "too many arguments");
  success = 1;
  for (n = 2; nargs-- && success; n++) {
    if (lua_type(L, n) == LUA_TNUMBER) {
      size_t l = (size_t)luaL_checkinteger(L, n);
      success = read_chars(L, &f->file, l);
    } else {
      const char *p = luaL_checkstring(L, n);
      if (*p == '*')
        p++; /* skip optional '*' (for compatibility) */
      switch (*p) {
      case 'a':                /* file */
        read_all(L, &f->file); /* read entire file */
        success = 1;           /* always success */
        break;
      case 'n': /* number */
      case 'l': /* line */
      case 'L': /* line with end-of-line */
      default:
        return luaL_argerror(L, n, "invalid format");
      }
    }
  }

  if (!success) {
    lua_pop(L, 1);  /* remove last result */
    lua_pushnil(L); /* push nil instead */
  }

  return n - 2;
}

static int luavgl_fs_write(lua_State *L)
{
  luavgl_fs_t *f = luavgl_to_fs(L, 1);
  lua_pushvalue(L, 1); /* push file at the stack top (to be returned) */

  int arg = 2;
  int nargs = lua_gettop(L) - arg;
  int status = 1;
  lv_fs_res_t res;
  size_t l;
  uint32_t nw;
  const char *s;

  for (; nargs--; arg++) {
    if (lua_type(L, arg) == LUA_TNUMBER) {
      s = luaL_tolstring(L, arg, &l);
      res = lv_fs_write(&f->file, s, l, &nw);
      lua_pop(L, 1);
    } else {
      s = luaL_checklstring(L, arg, &l);
      res = lv_fs_write(&f->file, s, l, &nw);
    }
    status = status && nw == l && res == LV_FS_RES_OK;
  }

  if (status) {
    return 1; /* file handle already on stack top */
  }

  lua_pushnil(L);
  lua_pushstring(L, "failed"); /* no details */
  lua_pushinteger(L, res);
  return 3;
}

static int luavgl_fs_close(lua_State *L)
{
  luavgl_fs_t *f = luavgl_to_fs(L, 1);

  LV_LOG_INFO("enter");
  f->closed = true;
  lv_fs_close(&f->file);

  return 0;
}

static int luavgl_fs_seek(lua_State *L)
{
  static const int mode[] = {LV_FS_SEEK_SET, LV_FS_SEEK_CUR, LV_FS_SEEK_END};
  static const char *const modenames[] = {"set", "cur", "end", NULL};
  luavgl_fs_t *f = luavgl_to_fs(L, 1);
  int op = luaL_checkoption(L, 2, "cur", modenames);
  lua_Integer p3 = luaL_optinteger(L, 3, 0);
  uint32_t offset = (uint32_t)p3;
  luaL_argcheck(L, (lua_Integer)offset == p3, 3,
                "not an integer in proper range");

  lv_fs_res_t res = lv_fs_seek(&f->file, offset, mode[op]);
  if (res != LV_FS_RES_OK) {
    lua_pushnil(L);

    lua_pushstring(L, "failed");
    lua_pushinteger(L, res);
    return 3;
  }

  uint32_t pos;
  res = lv_fs_tell(&f->file, &pos);
  if (res != LV_FS_RES_OK) {
    lua_pushinteger(L, 0);
  } else {
    lua_pushinteger(L, (lua_Integer)pos);
  }
  return 1;
}

static int luavgl_fs_tostring(lua_State *L)
{
  lua_pushfstring(L, "lv_fs handler: %p\n", luavgl_to_fs(L, 1));
  return 1;
}

static int luavgl_fs_gc(lua_State *L)
{
  LV_LOG_INFO("enter");

  luavgl_fs_t *v = luaL_checkudata(L, 1, "lv_fs");
  if (!v->closed) {
    v->closed = true;
    lv_fs_close(&v->file);
  }

  return 0;
}

/**
 * luavgl.open_dir(path)
 */
static int luavgl_dir_open(lua_State *L)
{
  const char *path = lua_tostring(L, 1);

  luavgl_dir_t *d = lua_newuserdata(L, sizeof(luavgl_dir_t));
  d->closed = false;

  lv_fs_res_t res = lv_fs_dir_open(&d->dir, path);
  if (res != LV_FS_RES_OK) {
    LV_LOG_ERROR("open failed: %s", path);
    lua_pushnil(L);
    lua_pushfstring(L, "open failed: %s\n", path);
    lua_pushinteger(L, res); /* return lv_fs error number */
    return 3;
  }

  luaL_getmetatable(L, "lv_dir");
  lua_setmetatable(L, -2);

  return 1;
}

static int luavgl_dir_read(lua_State *L)
{
  luavgl_dir_t *d = luavgl_to_dir(L, 1);
  char buffer[PATH_MAX];
  lv_fs_res_t res = lv_fs_dir_read(&d->dir, buffer, sizeof(buffer));
  if (res != LV_FS_RES_OK || buffer[0] == '\0') {
    lua_pushnil(L);
  } else {
    lua_pushstring(L, buffer);
  }

  return 1;
}

static int luavgl_dir_close(lua_State *L)
{
  LV_LOG_INFO("error");
  luavgl_dir_t *d = luavgl_to_dir(L, 1);
  d->closed = true;
  lv_fs_dir_close(&d->dir);
  return 0;
}

static int luavgl_dir_tostring(lua_State *L)
{
  lua_pushfstring(L, "lv_fs dir handler: %p\n", luavgl_to_dir(L, 1));
  return 1;
}

static int luavgl_dir_gc(lua_State *L)
{
  LV_LOG_INFO("enter");

  luavgl_dir_t *v = luaL_checkudata(L, 1, "lv_dir");
  if (!v->closed) {
    v->closed = true;
    lv_fs_dir_close(&v->dir);
  }

  return 0;
}

/**
 * luavgl.fs lib
 *
 */
static const luaL_Reg fs_lib[] = {
    {"open_file", luavgl_fs_open },
    {"open_dir",  luavgl_dir_open},

    {NULL,        NULL           },
};

/*
** methods for file handles
*/
static const luaL_Reg fs_methods[] = {
    {"read",  luavgl_fs_read },
    {"write", luavgl_fs_write},
    {"close", luavgl_fs_close},
    {"seek",  luavgl_fs_seek },

    {NULL,    NULL           },
};

static const luaL_Reg fs_meta[] = {
    {"__gc",       luavgl_fs_gc      },
    {"__close",    luavgl_fs_gc      },
    {"__tostring", luavgl_fs_tostring},
    {"__index",    NULL              }, /* place holder */

    {NULL,         NULL              }
};

/*
** methods for dir handles
*/
static const luaL_Reg dir_methods[] = {
    {"read",  luavgl_dir_read },
    {"close", luavgl_dir_close},

    {NULL,    NULL            },
};

static const luaL_Reg dir_meta[] = {
    {"__gc",       luavgl_dir_gc      },
    {"__close",    luavgl_dir_gc      },
    {"__tostring", luavgl_dir_tostring},
    {"__index",    NULL               }, /* place holder */

    {NULL,         NULL               }
};

static void luavgl_fs_init(lua_State *L)
{
  /* create lv_fs metatable */
  luaL_newmetatable(L, "lv_fs");
  luaL_setfuncs(L, fs_meta, 0);

  luaL_newlib(L, fs_methods);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop metatable */

  /* create lv_dir metatable */
  luaL_newmetatable(L, "lv_dir");
  luaL_setfuncs(L, dir_meta, 0);

  luaL_newlib(L, dir_methods);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop metatable */

  /* luavgl.fs lib */
  luaL_newlib(L, fs_lib);
  lua_setfield(L, -2, "fs");
}
