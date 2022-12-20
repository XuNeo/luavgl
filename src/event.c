#include <assert.h>

#include "lugl.h"
#include "private.h"

/* Processes a result and pushes the data onto the stack
   returns the number of items pushed */
static int push_event_data(lua_State *L, lv_event_code_t event)
{
  switch (event) {
  case LV_EVENT_PRESSED:
    /* code */
    lua_pushstring(L, "pressed");
    return 1;
    break;

  default:
    return 0;
    break;
  }

  return 0;
}

static void lugl_obj_event_cb(lv_event_t *e)
{
  lua_State *L = e->user_data;
  if (L == NULL) {
    debug("Null user data, should be L.\n");
  }

  lv_obj_t *obj = e->target;

  lugl_obj_data_t *data = obj->user_data;
  if (data == NULL)
    return;

  int ref = LUA_NOREF;
  for (int i = 0; i < data->n_events; i++) {
    if (data->events[i].code == LV_EVENT_ALL ||
        data->events[i].code == e->code) {
      ref = data->events[i].ref;
      break;
    }
  }

  if (ref == LUA_NOREF) {
    /* nobody cares this event, something went wrong but can be ignored. */
    return;
  }

  lua_pushinteger(L, e->code);
  int nargs = push_event_data(L, e->code) + 1;
  lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
  lua_insert(L, -1 - nargs);

  lua_call(L, nargs, 0);
}

static void lugl_obj_remove_event(lua_State *L, lv_obj_t *obj,
                                  struct event_callback_s *event)
{
  luaL_unref(L, LUA_REGISTRYINDEX, event->ref);
  event->code = -1; /* mark it as unused. */
  event->ref = LUA_NOREF;
  lv_obj_remove_event_dsc(obj, event->dsc);
  event->dsc = NULL;
}

/* img.onevent(const.event.PRESSED, function(code, value) -- end) */
static int lugl_obj_on_event(lua_State *L)
{
  bool remove_all; /* if third parameter is noneornil, remove all events. */

  lv_obj_t *obj = lugl_check_obj(L, 1);
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
  lugl_obj_data_t *data = obj->user_data;

  /* check if event code already added, find a slot to store this callback */
  int slot = 0;
  if (data && data->events) {
    for (; slot < data->n_events; slot++) {
      struct event_callback_s *event = &data->events[slot];
      if (event->code == code) { /* same event can only be added once. */
        lugl_obj_remove_event(L, obj, event);
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

  /* create obj->user_data if NULL */
  if (data == NULL) {
    data = lugl_obj_alloc_data(L, obj);
  }

  struct event_callback_s *events = data->events;

  /* create obj->data->events, if NULL, realloc if existing and find no slot
   */
  if (events == NULL) {
    events = calloc(sizeof(struct event_callback_s), 1);
    if (events == NULL) {
      return luaL_error(L, "No memory.");
    }

    data->events = events;
    data->n_events = 1;
  } else {
    /* realloc? */
    if (slot && slot == data->n_events) {
      struct event_callback_s *_events;
      _events = realloc(data->events, (data->n_events + 1) * sizeof(*_events));
      if (_events == NULL) {
        return luaL_error(L, "No memory.");
      }
      events = _events;
      data->n_events++; /* now we have +1 event */
      data->events = events;
    }
    /* else: we have found a slot to reuse, use it. */
  }

  /* setup event callback */

  void *dsc = lv_obj_add_event_cb(obj, lugl_obj_event_cb, code, L);
  struct event_callback_s *event = &events[slot];
  event->code = code;
  event->ref = lugl_check_continuation(L, 3);
  event->dsc = dsc;

  return 0;
}

static void lugl_obj_event_init(lv_obj_t *obj)
{
  lugl_obj_data_t *data = obj->user_data;

  data->n_events = 0;
}

/**
 * Remove all events added, and free memory of events
 */
static void lugl_obj_remove_event_all(lua_State *L, lv_obj_t *obj)
{
  lugl_obj_data_t *data = obj->user_data;
  if (data == NULL || data->events == NULL) {
    return;
  }

  struct event_callback_s *events = data->events;

  int i = 0;
  for (; i < data->n_events; i++) {
    struct event_callback_s *event = &data->events[i];
    if (event->code != -1) {
      lugl_obj_remove_event(L, obj, event);
    }
  }

  free(events);
  data->n_events = 0;
  data->events = NULL;
}
