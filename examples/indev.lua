local function indev_example()
    local indev = lugl.indev.get_act()
    print("act indev: ", indev)

    local obj_act = lugl.indev.get_obj_act()
    print("obj_act: ", obj_act)

    local root = lugl.Object(nil, {
            w = lugl.PCT(30),
            h = lugl.PCT(30),
            align = lugl.ALIGN.CENTER,
            bg_color = "#aaa",
        })

    root:Object({
        w = 1000,
        h = 1000,
        align = lugl.ALIGN.CENTER,
        bg_color = "#555",
    }):onevent(lugl.EVENT.ALL, function(obj, code)
        local indev = lugl.indev.get_act()

        if not indev then return end

        if code == lugl.EVENT.PRESSED then
            local x, y = indev:get_point()
            print("pressed: ", x, y)
        end

        if code == lugl.EVENT.PRESSING then
            local x, y = indev:get_vect()
            print("vect: ", x, y)
        end

        if code == lugl.EVENT.GESTURE then
            local gesture_dir = indev:get_gesture_dir()
            print("gesture_dir: ", gesture_dir)
        end

        if code == lugl.EVENT.RELEASED then
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
