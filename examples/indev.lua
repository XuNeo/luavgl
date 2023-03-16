local function indev_example()
    local indev = lvgl.indev.get_act()
    print("act indev: ", indev)


    indev:on_event(lvgl.EVENT.SHORT_CLICKED, function ()
        print("indev pressed")
    end)
    local obj_act = lvgl.indev.get_obj_act()
    print("obj_act: ", obj_act)

    local root = lvgl.Object(nil, {
            w = lvgl.PCT(30),
            h = lvgl.PCT(30),
            align = lvgl.ALIGN.CENTER,
            bg_color = "#aaa",
        })

    root:Object({
        w = 1000,
        h = 1000,
        align = lvgl.ALIGN.CENTER,
        bg_color = "#555",
    }):onevent(lvgl.EVENT.ALL, function(obj, code)
        local indev = lvgl.indev.get_act()

        if not indev then return end

        if code == lvgl.EVENT.PRESSED then
            local x, y = indev:get_point()
            print("pressed: ", x, y)
        end

        if code == lvgl.EVENT.PRESSING then
            local x, y = indev:get_vect()
            print("vect: ", x, y)
        end

        if code == lvgl.EVENT.GESTURE then
            local gesture_dir = indev:get_gesture_dir()
            print("gesture_dir: ", gesture_dir)
        end

        if code == lvgl.EVENT.RELEASED then
            local scroll_dir = indev:get_scroll_dir()
            print("scroll_dir: ", scroll_dir)

            local scroll_obj = indev:get_scroll_obj()
            print("scroll_obj: ", scroll_obj)

            local x, y = indev:get_vect()
            local obj_search = obj:indev_search(x, y)
            print("indev search obj: ", obj_search)

            local obj_search = obj:indev_search({x, y})
            print("indev search obj2: ", obj_search)
        end
    end)
end

indev_example()
