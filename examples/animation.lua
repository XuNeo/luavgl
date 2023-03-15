local root = lvgl.Object()
root:set { w = lvgl.HOR_RES(), h = lvgl.VER_RES() }

-- create obj on root
local obj = root:Object()

-- create image on root and set position/img src/etc. properties.
local img = root:Image {
    src = SCRIPT_PATH .. "/assets/lvgl-logo.png",
    x = 0,
    y = 0,
    bg_color = 0x004400,
    pad_all = 0
}

-- change image properties.

img:set {
    src = SCRIPT_PATH .. "/assets/lvgl-logo.png",
    align = lvgl.ALIGN.CENTER,
}

-- create animation on object
local anim = img:anim {
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

lvgl.Timer {
    period = 1000,
    cb = function(t)
        print("stop animation in timer.")
        t:delete()
        print(anim)
        anim:stop()
        anim = nil
        collectgarbage("collect")
        collectgarbage("collect")
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
