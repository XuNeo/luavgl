local root = lvgl.Object()
root:set { w = lvgl.HOR_RES(), h = lvgl.VER_RES() }

-- create image on root and set position/img src/etc. properties.
root:Object():Image {
    src = SCRIPT_PATH .. "/assets/lvgl-logo.png",
    x = 0,
    y = 0,
    bg_color = 0x004400,
    pad_all = 0,
    align = lvgl.ALIGN.CENTER,
}:Anim {
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
    end,
    done_cb = function (anim, obj)
        print("anim done.: ", anim, "obj:", obj)
        anim:delete()
    end
}

-- second anim example with playback
local obj = root:Object {
    bg_color = "#F00000",
    radius = lvgl.RADIUS_CIRCLE,
    align = lvgl.ALIGN.LEFT_MID,
}
obj:clear_flag(lvgl.FLAG.SCROLLABLE)

--- @type AnimPara
local animPara = {
    run = true,
    start_value = 10,
    end_value = 50,
    duration = 1000,
    playback_delay = 100,
    playback_time = 500,
    repeat_count = lvgl.ANIM_REPEAT_INFINITE,
    path = "ease_in_out",
}

animPara.exec_cb = function(obj, value)
    obj:set { size = value }
end

obj:Anim(animPara)

animPara.end_value = 240
animPara.exec_cb = function(obj, value)
    obj:set { x = value }
end

obj:Anim(animPara)
