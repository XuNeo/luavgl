---@meta


---
--- Create Extension widget on parent
--- @param parent? Object | nil
--- @param property? ObjectStyle
--- @return Extension
function lugl.Extension(parent, property)
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
