local roller = lvgl.Roller(nil, {
    options = {
        options = "January\nFebruary\nMarch\nApril\nMay\nJune\nJuly\nAugust\nSeptember\nOctober\nNovember\nDecember",
        mode = lvgl.ROLLER_MODE.INFINITE,
    },
    visible_cnt = 4,
    align = lvgl.ALIGN.CENTER
})

print("create roller:", roller)
roller:onevent(lvgl.EVENT.VALUE_CHANGED, function (obj, code)
    print(obj:get_selected_str())
end)

