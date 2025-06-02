-- 让物体跟着鼠标移动
---@param object GObject
---@param event GEvent
local function UpdateMousePosition(object, event)
    if (event.type == EventType.Mouse.Motion) then
        -- 给物体一个对应的力
        local mouse = { event.mouse.xrel, event.mouse.yrel }
        mouse[1] = mouse[1] * DeltaTime
        mouse[2] = mouse[2] * DeltaTime
        object.RigidBody.applyImpulseToCenter(mouse)
    end
end

Event.registerCallback(Self, UpdateMousePosition)
