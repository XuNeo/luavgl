local roller = lugl.Roller(nil, {
    options = {
        options = "January\nFebruary\nMarch\nApril\nMay\nJune\nJuly\nAugust\nSeptember\nOctober\nNovember\nDecember",
        mode = lugl.ROLLER_MODE.INFINITE,
    },
    visible_cnt = 4,
    align = lugl.ALIGN.CENTER
})

print("create roller:", roller)
roller:onevent(lugl.EVENT.VALUE_CHANGED, function (obj, code)
    print(obj:get_selected_str())
end)

