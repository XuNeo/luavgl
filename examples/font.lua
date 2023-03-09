local label1 = lvgl.Label(nil, {
    x = 0, y = 0,
    text_font = lvgl.Font("montserrat", 32, "normal"),
    text = "Hello Font",
    align = lvgl.ALIGN.CENTER
})

print("label1: ", label1)

lvgl.Label(nil, {
    -- x= 0, y = 0,
    -- text_font = lvgl.Font("MiSansW medium, montserrat", 24),
    text_font = lvgl.BUILTIN_FONT.MONTSERRAT_22,
    text = "Hello Font2",
}):align_to({
    type = lvgl.ALIGN.OUT_BOTTOM_LEFT,
    base = label1,
})
