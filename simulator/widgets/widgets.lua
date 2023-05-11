---@meta

lvgl = require "lvgl"
---
--- Create Extension widget on parent
--- @param parent? Object | nil
--- @param property? ObjectStyle
--- @return Extension
function lvgl.Extension(parent, property)
end

--- Create Extension widget on obj
--- @param property? ObjectStyle
--- @return Extension
function obj:Extension(property)
end

---
--- Extension widget
---@class Extension:Object
---
local extension = {}

--- set method
--- @param p ObjectStyle
--- @return nil
function extension:set(p)
end


---
--- Create Pointer widget
--- @param parent? Object | nil
--- @param property? ObjectStyle
--- @return Pointer
function lvgl.Pointer(parent, property)
end

--- Create Extension widget on obj
--- @param property? ObjectStyle
--- @return Extension
function obj:Pointer(property)
end

---
--- Pointer widget
---@class Pointer:Object
---
local pointer = {}

--- set method
--- @param p ObjectStyle
--- @return nil
function pointer:set(p)
end


---
--- Create AnalogTime widget on parent
--- @param parent? Object | nil
--- @param property? AnalogTimeStyle
--- @return AnalogTime
function lvgl.AnalogTime(parent, property)
end

--- Create AnalogTime widget on obj
--- @param property? AnalogTimeStyle
--- @return AnalogTime
function obj:AnalogTime(property)
end

---
--- Extension widget
---@class AnalogTime:Object
---
local analogTime = {}

--- set method
--- @param p AnalogTimeStyle
--- @return nil
function analogTime:set(p)
end

--- Pause the hand refresh timer
function analogTime:pause()
end

--- Resume the hand refresh timer
function analogTime:resume()
end

--- Analog time hands parameter
--- @class AnalogTimeHands
--- @field hour string Image source path for hour hand
--- @field minute string minute hand
--- @field second string second hand
---


--- Analog time widget property
--- @class AnalogTimeStyle :StyleProp
--- @field hands AnalogTimeHands Hands images
--- @field period integer Timer refresh period, default to 1s/1min depending on whether there's second/min hands
---