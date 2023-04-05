local lvgl = require("lvgl")

local MOVE_SPEED = 480 / 8000 -- 8s for 480 pixel, pixel per ms
local PIXEL_PER_METER = 80
local TOP_Y = 20
local BOTTOM_Y = 480 - 112
local PIPE_COUNT = 5
local PIPE_GAP = 100
local PIPE_SPACE = 120

-- SCRIPT_PATH is set in simulator/main.c, used to get the abs path of first
-- lua script lua get called. In this example, SCRIPT_PATH is set to
-- path of `examples.lua`, flappyBird.lua is called when button is clicked.

local IMAGE_PATH = SCRIPT_PATH
if not IMAGE_PATH then
    IMAGE_PATH = "/"
    print("Note image root path is set to: ", IMAGE_PATH)
end

IMAGE_PATH = IMAGE_PATH .. "/flappyBird/"
print("IMAGE_PATH:", IMAGE_PATH)

local function randomY()
    return math.random(TOP_Y + 30, BOTTOM_Y - 50 - 50)
end

local function screenCreate(parent)
    local property = {
        w = 480,
        h = 480,
        bg_opa = 0,
        border_width = 0,
        pad_all = 0
    }

    local scr
    if parent then
        scr = parent:Object{
            w = 480,
            h = 480,
            bg_opa = 0,
            border_width = 0,
            pad_all = 0
        }
    else
        scr = lvgl.Object(nil, property)
    end
    scr:clear_flag(lvgl.FLAG.SCROLLABLE)
    scr:clear_flag(lvgl.FLAG.CLICKABLE)
    return scr
end

local function Image(parent, src)
    local img = {}
    img.widget = parent:Image{
        src = src
    }

    img.w, img.h = img.widget:get_img_size()
    if not img.w or not img.h then
        error("failed to load image: " .. src)
    end
    return img
end

local function ImageScroll(root, src, animSpeed, y)
    -- image on right
    local right = Image(root, src).widget
    right:set{
        src = src,
        x = 480,
        y = y,
        pad_all = 0
    }

    local img = Image(root, src).widget
    img:set{
        x = 0,
        y = y,
        src = src,
        pad_all = 0
    }

    img:Anim{
        run = true,
        start_value = 0,
        end_value = -480,
        time = 480 / animSpeed,
        repeat_count = lvgl.ANIM_REPEAT_INFINITE,
        path = "linear",
        exec_cb = function(obj, value)
            img:set{
                x = value
            }

            right:set{
                x = value + 480
            }
        end
    }

    img:clear_flag(lvgl.FLAG.CLICKABLE)

    return img
end

local function Frames(parent, src, fps)
    local frame = Image(parent, src[1])
    fps = fps ~= 0 and fps or 25

    frame.src = src
    frame.len = #src
    frame.i = 0

    frame.timer = lvgl.Timer {
        period = 1000 / fps,
        cb = function(t)
            frame.widget:set{
                src = frame.src[frame.i]
            }

            frame.i = frame.i + 1
            if frame.i == frame.len then
                frame.i = 1
            end
        end
    }

    frame.start = function(self)
        self.timer:resume()
    end

    frame.pause = function(self)
        self.timer:pause()
    end

    return frame
end

local function Pipe(parent)
    local up = Image(parent, IMAGE_PATH .. "pipe_up.png")
    local down = Image(parent, IMAGE_PATH .. "pipe_down.png")
    local pipe = {
        up = up.widget,
        down = down.widget,
        w = up.w,
        h = up.h,
        x = 0,
        y = 0
    }

    function pipe:updatePipePos()
        self.up:set{
            x = self.x,
            y = self.y - up.h
        }

        self.down:set{
            x = self.x,
            y = self.y + PIPE_GAP
        }
    end

    pipe:updatePipePos()
    return pipe
end

local function ObjInfo(x, y, w, h)
    return {
        x = x,
        y = y,
        w = w,
        h = h
    }
end

local function Pipes(parent)
    local pipes = {}

    -- add initial pipe
    for i = 1, PIPE_COUNT do
        pipes[i] = Pipe(parent)
        if i == 1 then
            pipes.w = pipes[i].w -- record pipe size
            pipes.h = pipes[i].h
        end
    end

    local function pipesPosinit()
        local x = 480;
        local y = randomY()

        for i = 1, PIPE_COUNT do
            local pipe = pipes[i]
            pipe.x = x
            pipe.y = y
            pipe:updatePipePos()
            pipes[i] = pipe
            x = x + PIPE_SPACE + pipe.w
            y = randomY()
        end
    end

    pipesPosinit()

    pipes.score = 0
    pipes.last = PIPE_COUNT -- first pipe index in pipes.pipes
    pipes.totalWidth = (PIPE_COUNT) * (PIPE_SPACE + pipes.w)
    pipes.birdInfo = ObjInfo(0, 0, 0, 0)
    pipes.gapInfo = ObjInfo(0, 0, 0, 0)

    function pipes:setObjInfo(x, y, w, h)
        self.birdInfo.x = x
        self.birdInfo.y = y
        if w then
            self.birdInfo.w = w
        end
        if h then
            self.birdInfo.h = h
        end
    end

    local function setGapInfo(x, y, w, h)
        pipes.gapInfo.x = x
        pipes.gapInfo.y = y
        pipes.gapInfo.w = w
        pipes.gapInfo.h = h
    end

    pipes.objPassing = -1

    local function isBirdCollision()
        local bird = pipes.birdInfo
        local gap = pipes.gapInfo

        -- far left
        if bird.x + bird.w < gap.x then
            return false
        end

        -- far right
        if bird.x > gap.x + gap.w then
            return false
        end

        -- in middle

        if (bird.y > gap.y) and (bird.y + bird.h < gap.y + gap.h) then
            return false
        end
        return true
    end

    local function moveVirtualX(dx)
        for i = 1, PIPE_COUNT do
            local pipe = pipes[i]
            local newX = pipe.x + dx

            if newX + pipes.w < 0 then
                newX = newX + pipes.totalWidth
                pipe.y = randomY()
                pipes.last = i
            end

            pipe.x = newX
            pipe.updatePipePos(pipe)
        end
    end

    local function checkScore(i)
        local pipe = pipes[i]

        local bird = pipes.birdInfo
        local gap = pipes.gapInfo
        local passing = pipes.objPassing

        -- far left or right
        if bird.x + bird.w < gap.x or bird.x > gap.x + gap.w then
            if passing > 0 and i == passing then
                pipes.score = pipes.score + 1
                passing = -1
                pipes.scoreUpdateCB(pipes.score)
            end
        else
            if passing < 0 then
                passing = i
            end
        end

        pipes.objPassing = passing
    end

    --- Detect if obj has collision with pipes
    local function collisionDetect()
        local first = (pipes.last % PIPE_COUNT) + 1
        for idx = 0, PIPE_COUNT - 1 do
            local i = (first + idx - 1) % PIPE_COUNT + 1
            local pipe = pipes[i]
            setGapInfo(pipe.x, pipe.y, pipe.w, PIPE_GAP)

            if isBirdCollision() then
                local bird = pipes.birdInfo
                if pipes.collisionCB then
                    pipes.collisionCB()
                end
            end

            checkScore(i)
        end
    end

    pipes.preValue = 0
    pipes.anim = pipes[1].up:Anim{
        run = false,
        start_value = 0,
        end_value = 480,
        time = 480 / MOVE_SPEED, -- MOVE_SPEED
        repeat_count = lvgl.ANIM_REPEAT_INFINITE,
        path = "linear",
        exec_cb = function(obj, value)
            local x = pipes.preValue
            local d
            if value < x then
                d = value + 480 - x
            else
                d = value - x
            end
            pipes.preValue = value
            moveVirtualX(-d)
            collisionDetect()
        end
    }

    function pipes:start()
        self.anim:start()
    end

    function pipes:stop()
        self.anim:stop()
    end

    function pipes:reset()
        pipesPosinit()
        pipes.score = 0
        pipes.preValue = 0
        pipes.objPassing = -1
    end

    function pipes:setCollisionCB(collisionCB)
        self.collisionCB = collisionCB
    end

    function pipes:setScoreUpdateCB(cb)
        self.scoreUpdateCB = cb
    end

    return pipes
end

local function Bird(parent, birdMovedCB)
    -- create bird Frame(sprite) in 5FPS
    local bird = Frames(parent,
        {IMAGE_PATH .. "bird1.png", IMAGE_PATH .. "bird2.png", IMAGE_PATH .. "bird3.png"}, 5)

    local function birdVarInit()
        bird.x = 240 - bird.w / 2
        bird.y = 240 - bird.h / 2
        bird.widget:set{
            x = bird.x,
            y = bird.y
        }

        bird.head = 0
        bird.force = 0 -- in unit of m/s^2 rather than N
        bird.velocity = 0 -- vertical verlocity
        bird.time = 0 -- time stamp when it updates
        bird.moving = false
    end

    birdVarInit()

    bird.setY = function(self)
        bird.widget:set{
            y = bird.y
        }
    end

    bird.setHead = function(self)
        bird.widget:set{
            angle = self.head
        }
    end

    bird.applyForce = function(self, force)
        self.force = force
        if bird.moving then
            return
        end

        bird.moving = true
        self.y_anim:start()
    end

    bird.pressed = function(self)
        bird:applyForce(-13)
        bird.velocity = 0
    end

    bird.released = function(self)
        bird:applyForce(9.8)
        bird.velocity = 0
    end

    local function velocity2HeadAngle(v)
        -- -9.8 ~ 9.8：90 ~ -90
        return v * 60
    end

    -- y moving anim, in time.
    bird.y_anim = bird.widget:Anim{
        run = false,
        start_value = 0,
        end_value = 1000,
        time = 1000, -- 1000 ms
        repeat_count = lvgl.ANIM_REPEAT_INFINITE,
        path = "linear",
        exec_cb = function(obj, tNow)
            -- we use anim to get current time, can calculate position based on force/velocity
            if tNow < bird.time then
                tNow = tNow + 1000
            end
            local y = bird.y
            local preT = bird.time
            local v = bird.velocity
            local t = tNow < preT and tNow + 1000 - preT or tNow - preT
            t = t * 0.001 -- ms to s

            v = bird.force * t + v
            if v > 10 then
                v = 10
            end

            if v < -10 then
                v = -10
            end

            y = y + v * t * PIXEL_PER_METER
            if y > BOTTOM_Y - 30 then
                y = BOTTOM_Y - 30
                v = 0
            end
            if y < TOP_Y then
                y = TOP_Y
                v = 0
            end

            bird.y = y
            bird.time = tNow
            bird.velocity = v
            bird.head = velocity2HeadAngle(v)

            birdMovedCB(bird.x, bird.y)
            -- set y
            bird:setY()
            bird:setHead()
        end
    }

    function bird:stop()
        bird.y_anim:stop()
    end

    function bird:gameOver()
        -- like it's released forever
        bird.released()
    end

    function bird:start()
        bird.y_anim:start()
    end

    function bird:reset()
        bird.stop()
        birdVarInit()
    end

    return bird;
end

local function Background(root, bgEventCB)
    local bgLayer = screenCreate(root) -- background layer
    bgLayer:add_flag(lvgl.FLAG.CLICKABLE) --  we accept event here

    local bg = ImageScroll(bgLayer, IMAGE_PATH .. "bg_day.png", MOVE_SPEED * 0.4, 0)
    local pipes = Pipes(bgLayer)
    local land = ImageScroll(bgLayer, IMAGE_PATH .. "land.png", MOVE_SPEED, BOTTOM_Y)

    bgLayer:onevent(lvgl.EVENT.PRESSED, function(obj, code)
        bgEventCB(lvgl.EVENT.PRESSED)
    end)

    bgLayer:onevent(lvgl.EVENT.RELEASED, function(obj, code)
        bgEventCB(lvgl.EVENT.RELEASED)
    end)

    return {
        pipes = pipes
    }
end

local function SysLayer(root)
    local sysLayer = screenCreate(root) -- upper layer
    return sysLayer
end

local function createPlayBtn(sysLayer, onEvent)
    local playBtn = Image(sysLayer, IMAGE_PATH .. "button_play.png").widget
    playBtn:add_flag(lvgl.FLAG.CLICKABLE)
    playBtn:set{
        align = {
            type = lvgl.ALIGN.CENTER,
            y_ofs = 80
        }
    }

    playBtn:onevent(lvgl.EVENT.PRESSED, onEvent)

    return playBtn
end

local function entry()
    local scr = screenCreate()
    local bgLayer
    local mainLayer
    local sysLayer
    local bird
    local pipes
    local bgEventCB -- background layer pressed/released event
    local birdMovedCB -- callback when bird position updates
    local collisionCB -- callback when collision happends
    local flagRunning = false
    local gameStart -- API to start game
    local gameOver -- API to stop game
    local scoreLabel
    local scoreBest = 0
    local scoreNow = 0
    local debouncing = false
    -- global event process

    local scoreUpdateCB = function(score)
        scoreLabel:set{
            text = string.format("%03d", score)
        }
        scoreNow = score
    end

    print("font:", lvgl.BUILTIN_FONT.MONTSERRAT_26)
    gameStart = function()
        if flagRunning then
            return
        end

        bird:reset()
        pipes:reset()
        pipes:start()
        bird:start()
        flagRunning = true
        scoreNow = 0
        if scoreLabel then
            scoreLabel:set{
                text = string.format("%03d", 0)
            }
        end
    end

    gameOver = function()
        if not flagRunning then
            return
        end

        debouncing = true
        flagRunning = false

        pipes:stop()
        bird:gameOver()
        if scoreNow > scoreBest then
            scoreBest = scoreNow
        end

        local gameoverImg = Image(sysLayer, IMAGE_PATH .. "text_game_over.png").widget
        gameoverImg:set{
            align = {
                type = lvgl.ALIGN.TOP_MID,
                y_ofs = 100
            }
        }

        gameoverImg:Anim{
            run = true,
            start_value = 0,
            end_value = 3600,
            time = 2000,
            repeat_count = 2,
            path = "bounce",
            exec_cb = function(obj, value)
                obj:set{
                    angle = value
                }
            end
        }

        local scoreImg = Image(sysLayer, IMAGE_PATH .. "score.png").widget
        scoreImg:set{
            align = {
                type = lvgl.ALIGN.CENTER,
                y_ofs = -20,
                x_ofs = 0
            }
        }
        scoreImg:Anim{
            run = true,
            start_value = 480,
            end_value = 0,
            time = 1000,
            repeat_count = 1,
            path = "ease_in",
            exec_cb = function(obj, value)
                obj:set{
                    align = {
                        type = lvgl.ALIGN.CENTER,
                        x_ofs = value,
                        y_ofs = -20
                    }
                }
            end
        }

        local scoreResultLabel = scoreImg:Label{
            text = string.format("%03d", scoreNow),
            text_font = lvgl.BUILTIN_FONT.MONTSERRAT_22,
            align = {
                type = lvgl.ALIGN.TOP_MID,
                x_ofs = 0,
                y_ofs = 25
            }
        }

        local scoreBestLabel = scoreImg:Label{
            text = string.format("%03d", scoreBest),
            text_font = lvgl.BUILTIN_FONT.MONTSERRAT_22,
            align = {
                type = lvgl.ALIGN.BOTTOM_MID,
                x_ofs = 0,
                y_ofs = -5
            }
        }
        scoreNow = 0

        local playBtn;
        playBtn = createPlayBtn(sysLayer, function(obj, code)
            if debouncing then
                return
            end

            gameStart()
            playBtn:delete()
            playBtn = nil
            gameoverImg:delete()
            gameoverImg = nil
            scoreImg:delete()
            scoreImg = nil
        end)

        lvgl.Timer {
            period = 1000,
            cb = function(t)
                t:delete()
                debouncing = false
            end
        }
    end

    bgEventCB = function(event)
        if not flagRunning then
            return
        end

        if event == lvgl.EVENT.PRESSED then
            bird:pressed()
        else
            bird:released()
        end
    end

    local birdMovedCB = function(x, y)
        pipes:setObjInfo(bird.x, bird.y) -- set intial bird position.
    end

    local collisionCB = function()
        print("bird collision, stop game")
        -- call later
        lvgl.Timer {
            period = 10,
            cb = function(t)
                t:delete()
                gameOver()
            end
        }

    end

    -- background layer, including sky, then pipes and land above
    bgLayer = Background(scr, bgEventCB) -- background layer
    pipes = bgLayer.pipes -- get pipes from bg layer for set bird info etc.
    pipes:setCollisionCB(collisionCB)
    pipes:setScoreUpdateCB(scoreUpdateCB)

    -- main layer, the bird
    mainLayer = screenCreate(scr) -- main layer
    bird = Bird(mainLayer, birdMovedCB)
    pipes:setObjInfo(bird.x, bird.y, bird.w, bird.h)
    -- system layer, score etc.
    sysLayer = SysLayer(scr)

    local title = Image(sysLayer, IMAGE_PATH .. "title.png").widget
    title:set{
        align = {
            type = lvgl.ALIGN.TOP_MID,
            y_ofs = 80
        }
    }

    local playBtn;
    playBtn = createPlayBtn(sysLayer, function()
        print("pressed")
        gameStart()
        playBtn:delete()
        playBtn = nil
        title:delete()
        title = nil

        local medal = Image(sysLayer, IMAGE_PATH .. "medals.png").widget
        medal:set{
            align = {
                type = lvgl.ALIGN.TOP_MID,
                y_ofs = 10,
                x_ofs = -50
            }
        }
        scoreLabel = sysLayer:Label{
            text = " 000",
            text_font = lvgl.BUILTIN_FONT.MONTSERRAT_28,
            align = {
                type = lvgl.ALIGN.TOP_MID,
                x_ofs = 10,
                y_ofs = 20
            }
        }
    end)
end

entry()

-- bird = Bird(, nil)
