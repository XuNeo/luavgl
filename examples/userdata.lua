-- user_data can store any type of lua value
local obj1 = lvgl.Label {
    text_color = "#333",
    border_color = "#f00",
    border_width = 1,
    align = lvgl.ALIGN.TOP_LEFT
}

obj1.user_data = 123.456
obj1.text = "user_data:\n" .. obj1.user_data

-- use user_data to store table
local obj2 = lvgl.Label {
    text_color = "#333",
    border_color = "#f00",
    border_width = 1,
    align = lvgl.ALIGN.TOP_RIGHT
}

obj2.user_data = {
    abc = "hello",
    def = "world",
    [1] = 123,
}
obj2.text = "user_data:"
for k, v in pairs(obj2.user_data) do
    print(k, v)
    obj2.text = obj2.text .. string.format("%s: %s\n", k, v)
end
