#include "luavgl.h"
#include "private.h"

static void luavgl_obj_event_cb(lv_event_t *e)
{
  lua_State *L = e->user_data;
  if (L == NULL) {
    debug("Null user data, should be L.\n");
  }

  int top = lua_gettop(L);
  lv_obj_t *obj = e->current_target;

  lua_pushlightuserdata(L, obj);
  lua_rawget(L, LUA_REGISTRYINDEX);
  luavgl_obj_t *lobj = luavgl_to_lobj(L, -1);
  if (lobj == NULL || lobj->obj == NULL)
    goto event_exit;

  int ref = LUA_NOREF;
  for (int i = 0; i < lobj->n_events; i++) {
    if (lobj->events[i].code == LV_EVENT_ALL ||
        lobj->events[i].code == e->code) {
      ref = lobj->events[i].ref;
      break;
    }
  }

  if (ref == LUA_NOREF) {
    /* nobody cares this event, something went wrong but can be ignored. */
    goto event_exit;
  }

  lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
  lua_pushlightuserdata(L, obj);
  lua_rawget(L, LUA_REGISTRYINDEX);

  lua_pushinteger(L, e->code);

  /* args: obj, code */
  luavgl_pcall_int(L, 2, 0);

event_exit:
  lua_settop(L, top);
}

static void luavgl_obj_remove_event(lua_State *L, lv_obj_t *obj,
                                    struct event_callback_s *event)
{
  luaL_unref(L, LUA_REGISTRYINDEX, event->ref);
  event->code = -1; /* mark it as unused. */
  event->ref = LUA_NOREF;
  lv_obj_remove_event_dsc(obj, event->dsc);
  event->dsc = NULL;
}

/* obj:onevent(luavgl.EVENT.PRESSED, function(code, value) -- end) */
static int luavgl_obj_on_event(lua_State *L)
{
  bool remove_all; /* if third parameter is noneornil, remove all events. */

  luavgl_obj_t *lobj = luavgl_to_lobj(L, 1);
  lv_obj_t *obj = lobj->obj;
  if (obj == NULL) {
    luaL_argerror(L, 1, "expect obj userdata.\n");
    return 0;
  }

  lv_event_code_t code = lua_tointeger(L, 2);
  if (code >= _LV_EVENT_LAST) {
    luaL_argerror(L, 2, "event code illegal");
    return 0;
  }

  remove_all = lua_isnoneornil(L, 3);

  /* check if event code already added, find a slot to store this callback */
  int slot = 0;
  if (lobj && lobj->events) {
    for (; slot < lobj->n_events; slot++) {
      struct event_callback_s *event = &lobj->events[slot];
      if (event->code == code) { /* same event can only be added once. */
        luavgl_obj_remove_event(L, obj, event);
        if (remove_all)
          continue; /* continue to remove all events associated. */
        else
          break; /* use this slot for our new event callback */
      }

      if (event->code == -1) {
        /* this callback has been removed, thus, we can use this slot */
        break;
      }
    }
  }

  if (remove_all) /* no need to add, just return */
    return 0;

  struct event_callback_s *events = lobj->events;

  /* create obj->lobj->events, if NULL, realloc if existing and find no slot
   */
  if (events == NULL) {
    events = calloc(sizeof(struct event_callback_s), 1);
    if (events == NULL) {
      return luaL_error(L, "No memory.");
    }

    lobj->events = events;
    lobj->n_events = 1;
  } else {
    /* realloc? */
    if (slot && slot == lobj->n_events) {
      struct event_callback_s *_events;
      _events = realloc(lobj->events, (lobj->n_events + 1) * sizeof(*_events));
      if (_events == NULL) {
        return luaL_error(L, "No memory.");
      }
      events = _events;
      lobj->n_events++; /* now we have +1 event */
      lobj->events = events;
    }
    /* else: we have found a slot to reuse, use it. */
  }

  /* setup event callback */

  void *dsc = lv_obj_add_event_cb(obj, luavgl_obj_event_cb, code, L);
  struct event_callback_s *event = &events[slot];
  event->code = code;
  event->ref = luavgl_check_continuation(L, 3);
  event->dsc = dsc;

  return 0;
}

/* obj:onClicked(function(code, value) end) */
static int luavgl_obj_on_clicked(lua_State *L)
{
  /* stack: obj, function cb */
  lua_pushinteger(L, LV_EVENT_CLICKED);
  lua_insert(L, 2);

  return luavgl_obj_on_event(L);
}

/* obj:onShortClicked(function(code, value) end) */
static int luavgl_obj_on_short_clicked(lua_State *L)
{
  /* stack: obj, function cb */
  lua_pushinteger(L, LV_EVENT_SHORT_CLICKED);
  lua_insert(L, 2);

  return luavgl_obj_on_event(L);
}

/* obj:onPressed(function(code, value) end) */
static int luavgl_obj_on_pressed(lua_State *L)
{
  lua_pushinteger(L, LV_EVENT_PRESSED);
  lua_insert(L, 2);

  return luavgl_obj_on_event(L);
}

static void luavgl_obj_event_init(luavgl_obj_t *lobj) { lobj->n_events = 0; }

/**
 * Remove all events added, and free memory of events
 */
static void luavgl_obj_remove_event_all(lua_State *L, luavgl_obj_t *lobj)
{
  if (lobj == NULL || lobj->events == NULL) {
    return;
  }

  struct event_callback_s *events = lobj->events;

  int i = 0;
  for (; i < lobj->n_events; i++) {
    struct event_callback_s *event = &lobj->events[i];
    if (event->code != -1) {
      luavgl_obj_remove_event(L, lobj->obj, event);
    }
  }

  free(events);
  lobj->n_events = 0;
  lobj->events = NULL;
}
