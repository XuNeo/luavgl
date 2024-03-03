
local scr = lvgl.disp.get_scr_act()
local indev = lvgl.indev.get_act()
print("diplay screen_act:", scr, "indev:", indev)

scr:onevent(lvgl.EVENT.GESTURE, function (obj, code)
    print("gesture dir: ", indev:get_gesture_dir())
end)

scr:onevent(lvgl.EVENT.CLICKED, function (obj, code)
    print("clicked: " .. code)
end)
