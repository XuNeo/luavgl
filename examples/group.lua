local function group_example()
    local g = lugl.group.create()
    g:set_default()

    -- for demo purpose, set all indev to use this group
    local indev = nil
    while true do
        indev = lugl.indev.get_next(indev)
        if not indev then break end

        local t = indev:get_type()
        if t == 2 or t == 4 then
            indev:set_group(g)
        end
    end

    local style = lugl.Style({
            border_width = 5,
            border_color = "#a00",
        })

    local root = lugl.Object(nil, {
            w = lugl.PCT(100),
            h = lugl.PCT(100),
            align = lugl.ALIGN.CENTER,
            bg_color = "#aaa",
            flex = {
                flex_direction = "row",
                flex_wrap = "wrap"
            }
        })

    root:add_style(style, lugl.STATE.FOCUSED)

    for _ = 1, 5 do
        local obj = root:Object({
                w = lugl.PCT(50),
                h = lugl.PCT(50),
                bg_color = "#555",
            })

        obj:add_style(style, lugl.STATE.FOCUSED)

        obj:onClicked(function(obj, code)
            print("clicked: ", obj)
        end)

        obj:onevent(lugl.EVENT.FOCUSED, function(obj, code)
            print("focused: ", obj)
            obj:scroll_to_view(true)
        end)

        obj:onevent(lugl.EVENT.DEFOCUSED, function(obj, code)
            print("defocused: ", obj)
        end)

        g:add_obj(obj)
    end
end

group_example()
