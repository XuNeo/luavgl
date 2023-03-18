# lvgl-lua-binding
lua + lvgl = luavgl

luavgl is currently under development. A flappy bird game is ready for showoff. See simulator instructions below.

<p align="center">
  <img src="https://i.ibb.co/nbgYvZW/flappybird.gif" />
</p>

## Introduction

luavgl is a wrapper around lvgl core functions and widgets with class inherence in mind, which is lvgl trying to do in `C`. Lua makes widgets inherence smoothly.

```lua
local root = lvgl.Object()
root:set { w = lvgl.HOR_RES(), h = lvgl.VER_RES() }

-- create obj on root
local obj = root:Object()

-- create image on root and set position/img src/etc. properties.
local img = root:Image {
    src = "res/image.png",
    x = 0,
    y = 0,
    bg_color = "#112233" -- #RRGGBB, 0xRRGGBB or "#RGB",
    pad_all = 0
}

-- change image properties.

img:set {
    src = "/assets/lvgl-logo.png",
    align = lvgl.ALIGN.CENTER,
}

-- create animation on object
img:Anim {
    run = true,
    start_value = 0,
    end_value = 3600,
    duration = 2000,
    repeat_count = 2,
    path = "bounce",
    exec_cb = function(obj, value)
        obj:set {
            angle = value
        }
    end
}

-- create Label on root and set its font
local label = root:Label {
    text = string.format("Hello %03d", 123),
    text_font = lvgl.Font("montserrat", 24, "normal"),
    align = {
        type = lvgl.ALIGN.CENTER,
        x_ofs = 0,
        y_ofs = 100,
    }
}


```

luavgl mainly targets for embedded device, a simulator on Ubuntu has been provided for preview.

### Embedded device

For embedded device, assume the lvgl environment has already been setup, simply add the `luavgl.c` to sources for compiling. And make sure `luaopen_lvgl` is added to global lib. Below is example from `simulator/main.c` shows this exact method.

```c
  luaL_requiref(L, "lvgl", luaopen_luavgl, 1);
  lua_pop(L, 1);
```

### PC simulator

Currently compile luavgl to `so` is not available, luavgl depends on lvgl and various configurations(`lv_conf.h`).
The simulator provided in this repo can be used as example if luavgl is required on PC.

Make sure clone the submodules, luavgl simulator comes directly from lvgl simulator with lua added.

```
git clone --recursive https://github.com/XuNeo/luavgl.git
```

#### Dependencies

To run simulator on PC, make sure `lua` header is available, you may need to install below packages.

```
sudo apt install libsdl2-dev lua5.3 liblua5.3-dev
```

Both lua5.3 and lua5.4 are supported. Versions below 5.2 has not been verified.

#### Build and run

```bash
cmake -Bbuild -DBUILD_SIMULATOR=ON
cd build
make
make run # run simulator
```
## Custom Widget

luavgl support custom widget, and use them in `lua` just like lvgl core widgets.
An example is provided in [`simulator/extension.c`](https://github.com/XuNeo/luavgl/blob/master/simulator/extension.c#L62)

For custom widget, it should be registered to Lua after luavgl lib loaded.

