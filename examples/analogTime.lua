local analogTime = lvgl.AnalogTime {
    border_width = 1,
    border_color = "#F00",
    x = lvgl.HOR_RES() // 2,
    y = lvgl.VER_RES() // 2,
    hands = {
        hour = SCRIPT_PATH .. "/assets/hand-hour.png",
        minute = SCRIPT_PATH .. "/assets/hand-minute.png",
        second = SCRIPT_PATH .. "/assets/hand-second.png",
    },
    period = 33,
}

print("analogTime: ", analogTime)
