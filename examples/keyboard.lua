local root = lvgl.Object(nil, {
    w = lvgl.HOR_RES(),
    h = lvgl.VER_RES(),
    pad_all = 0,
    bg_color = "#333",
    bg_opa = lvgl.OPA(50),
})
root:add_flag(lvgl.FLAG.CLICKABLE)

local ta = root:Textarea {
    password_mode = false,
    one_line = true,
    text_font = lvgl.Font("montserrat", 22),
    text_color = "#FFF",
    text = "Input text here",
    w = lvgl.SIZE_CONTENT,
    h = lvgl.SIZE_CONTENT,
    bg_opa = 0,
    border_width = 2,
    pad_all = 2,
    align = lvgl.ALIGN.TOP_MID,
}

print("created textarea: ", ta)

local keyboard = root:Keyboard {
    textarea = ta,
    align = lvgl.ALIGN.BOTTOM_MID,
}
print("created keyboard: ", keyboard)

ta:onevent(lvgl.EVENT.PRESSED, function(obj, code)
    keyboard:clear_flag(lvgl.FLAG.HIDDEN)
end)

ta:onevent(lvgl.EVENT.READY, function()
    keyboard:add_flag(lvgl.FLAG.HIDDEN)
end)

ta:onevent(lvgl.EVENT.CANCEL, function(obj, code)
    keyboard:add_flag(lvgl.FLAG.HIDDEN)
end)

root:onClicked(function (obj, code)
    keyboard:add_flag(lvgl.FLAG.HIDDEN)
end)
