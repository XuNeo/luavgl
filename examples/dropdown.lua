-- a dropdown on top left
local dd = lvgl.Dropdown(nil, {
    -- note there are two "Orange"
    options = "Apple\nBanana\nOrange\nCherry\nGrape\nRaspberry\nMelon\nOrange\nLemon\nNuts",
    symbol = "\xEF\x81\x94",
    dir = lvgl.DIR.RIGHT,
    highlight = false,
    text = nil,
    align = {
        type = lvgl.ALIGN.TOP_LEFT,
        x_ofs = 20,
        y_ofs = 20,
    }
})
local list = dd:get("list")
print("get dropdown list: ", list)
list:set { text_font = lvgl.BUILTIN_FONT.MONTSERRAT_20 }

print("available options:", dd:get("options"))

local cnt = 0
dd:onevent(lvgl.EVENT.VALUE_CHANGED,
    ---comment
    ---@param obj Dropdown
    ---@param code ObjEventCode
    function(obj, code)
        obj:add_option(string.format("Option:%d", cnt), dd:get("option_cnt"))
        cnt = cnt + 1
        print(obj:get("selected_str"), "option_cnt" .. obj:get("option_cnt"))
    end
)

-- another dropdown on top left
local dd = lvgl.Dropdown(nil, {
    options = "Apple\nBanana\nOrange\nCherry\nGrape\nRaspberry\nMelon\nOrange\nLemon\nNuts",
    symbol = "\xEF\x81\xB7",
    dir = lvgl.DIR.BOTTOM,
    text = "SetText",
    selected = 5,
    text_font = lvgl.BUILTIN_FONT.MONTSERRAT_20,
    align = {
        type = lvgl.ALIGN.BOTTOM_MID,
        x_ofs = 0,
        y_ofs = -20,
    }
})


dd:get("list"):set { text_font = lvgl.Font("montserrat", 24) }
dd:onevent(lvgl.EVENT.VALUE_CHANGED, function(obj, code)
    print(obj:get("selected_str") .. ":" .. obj:get("selected"),
        "dir:" .. obj:get("dir"),
        "option_index" .. dd:get("option_index", obj:get("selected_str")))
end)



lvgl.Timer {
    period = 1000,
    cb = function(t)
        t:delete()
        dd:open()
        print("now dd should be opened: " .. (dd:is_open() and "open" or "closed"))
    end
}

lvgl.Timer {
    period = 2000,
    cb = function(t)
        t:delete()
        dd:close()
        print("now dd should be closed: " .. (dd:is_open() and "open" or "closed"))
    end
}
