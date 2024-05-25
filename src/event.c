#include "luavgl.h"
#include "private.h"

static void luavgl_obj_event_cb(lv_event_t *e)
{
  struct event_callback_s *event = e->user_data;
  if (event == NULL)
    return;

  lua_State *L = event->L;
  int top = lua_gettop(L);
  lv_obj_t *obj = e->current_target;

  lua_pushlightuserdata(L, obj);
  lua_rawget(L, LUA_REGISTRYINDEX);
  luavgl_obj_t *lobj = luavgl_to_lobj(L, -1);
  if (lobj == NULL || lobj->obj == NULL) {
    lua_settop(L, top);
    return;
  }

  int ref = event->ref;
  if (ref == LUA_NOREF) {
    /* nobody cares this event, something went wrong but can be ignored. */
    lua_settop(L, top);
    return;
  }

  lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
  lua_pushlightuserdata(L, obj);
  lua_rawget(L, LUA_REGISTRYINDEX);

  lua_pushinteger(L, e->code);

  /* args: obj, code */
  luavgl_pcall_int(L, 2, 0);
}

/* obj:onevent(luavgl.EVENT.PRESSED, function(code, value) -- end) */
static int luavgl_obj_on_event(lua_State *L)
{
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

  int size = lv_array_size(&lobj->events);
  struct event_callback_s *event = NULL;
  struct event_callback_s **events = lv_array_front(&lobj->events);

  /* if third parameter is none or nil, remove this events */
  if (lua_isnoneornil(L, 3)) {
    for (int i = 0; i < size; i++) {
      event = events[i];
      if (event->code == code) {
        events[i] = NULL; /* Remove it from array */
        lv_result_t res = lv_obj_remove_event_dsc(lobj->obj, event->dsc);
        luaL_unref(L, LUA_REGISTRYINDEX, event->ref);
        event->dsc = NULL;
        event->L = NULL;
        event->ref = LUA_NOREF;
        event->code = _LV_EVENT_LAST;
        if (res != LV_RESULT_OK) {
          return luaL_error(L, "Failed to remove event dsc: %d\n", res);
        }

        return 0;
      }
    }

    return luaL_error(L, "No such event to remove: %d", code);
  }

  /* Check if the event code already exists, only one callback per code. */
  for (int i = 0; i < size; i++) {
    if (events[i]->code == code) {
      luaL_unref(L, LUA_REGISTRYINDEX, event->ref);
      event = events[i];
      break;
    }

    if (events[i]->code == _LV_EVENT_LAST) {
      /* code marked as _LV_EVENT_LAST means this event has been removed, we can
       * reuse it. */
      event = events[i];
      break;
    }
  }

  if (event == NULL) {
    /* Create a new one if not exist */
    event = lv_malloc_zeroed(sizeof(*event));
    if (event == NULL) {
      return luaL_error(L, "No memory");
    }

    lv_array_push_back(&lobj->events, &event);
    LV_LOG_INFO("obj: %p, push back event: %d", obj, code);
  }

  event->code = code;
  event->L = L;
  event->ref = luavgl_check_continuation(L, 3);
  event->dsc = lv_obj_add_event_cb(obj, luavgl_obj_event_cb, code, event);
  if (event->dsc == NULL) {
    lv_free(event);
    return luaL_error(L, "Failed to add event callback");
  }

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

/**
 * Remove all events added, and free memory of events
 */
static void luavgl_obj_remove_event_all(lua_State *L, luavgl_obj_t *lobj)
{
  if (lobj == NULL) {
    return;
  }

  int size = lv_array_size(&lobj->events);
  struct event_callback_s *event;
  struct event_callback_s **events = lv_array_front(&lobj->events);
  for (int i = 0; i < size; i++) {
    event = events[i];
    if (event == NULL) {
      continue;
    }

    events[i] = NULL;

    if (event->dsc == NULL) {
      continue;
    }

    lv_result_t res = lv_obj_remove_event_dsc(lobj->obj, event->dsc);
    if (res != LV_RESULT_OK) {
      LV_LOG_WARN("Failed to remove event dsc: %d", res);
      /* Ignore this error, remove from it anyway */
    }

    luaL_unref(L, LUA_REGISTRYINDEX, event->ref);
    lv_free(event);
  }

  lv_array_deinit(&lobj->events);
}
