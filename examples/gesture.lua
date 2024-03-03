local scr = lvgl.disp.get_scr_act()

-- For lvgl v8, need manually to add clickable flag.
scr:add_flag(lvgl.FLAG.CLICKABLE)

scr:onevent(lvgl.EVENT.GESTURE, function(obj, code)
    local indev = lvgl.indev.get_act()
    print("gesture dir: ", indev:get_gesture_dir())
    indev:wait_release()
end)

scr:onevent(lvgl.EVENT.CLICKED, function(obj, code)
    print("clicked: " .. code)
end)
