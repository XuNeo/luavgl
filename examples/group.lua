local function group_example()
    local g = lvgl.group.create()
    g:set_default()

    -- for demo purpose, set all indev to use this group
    local indev = nil
    while true do
        indev = lvgl.indev.get_next(indev)
        if not indev then break end

        local t = indev:get_type()
        if t == 2 or t == 4 then
            indev:set_group(g)
        end
    end

    local style = lvgl.Style({
            border_width = 5,
            border_color = "#a00",
        })

    local root = lvgl.Object(nil, {
            w = lvgl.PCT(100),
            h = lvgl.PCT(100),
            align = lvgl.ALIGN.CENTER,
            bg_color = "#aaa",
            flex = {
                flex_direction = "row",
                flex_wrap = "wrap"
            }
        })

    root:add_style(style, lvgl.STATE.FOCUSED)

    for _ = 1, 5 do
        local obj = root:Object({
                w = lvgl.PCT(50),
                h = lvgl.PCT(50),
                bg_color = "#555",
            })

        obj:add_style(style, lvgl.STATE.FOCUSED)

        obj:onClicked(function(obj, code)
            print("clicked: ", obj)
        end)

        obj:onevent(lvgl.EVENT.FOCUSED, function(obj, code)
            print("focused: ", obj)
            obj:scroll_to_view(true)
        end)

        obj:onevent(lvgl.EVENT.DEFOCUSED, function(obj, code)
            print("defocused: ", obj)
        end)

        g:add_obj(obj)
    end
end

group_example()
