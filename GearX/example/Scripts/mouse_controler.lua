---@param object GObject
---@param event GEvent
local function MouseHandle(object, event)
    -- 这里可以添加鼠标输入处理逻辑
    -- 例如，点击事件、拖动事件等
    if GlobalContext.IsMousePressed == nil then
        GlobalContext.IsMousePressed = {}
    end
    -- 检查对象ID
    if object.isValid() and object.getID() == event.object.id and object.isValidTexture() then
        --- 单次点击
        if event.type == EventType.Object.MouseClick then
            -- 处理鼠标点击事件,左键点击
            if event.object.button == 1 then
                playClickChunk()
            end
        end
        --- 按压事件
        if event.type == EventType.Object.MousePress then
            local color = object.Texture.getColorMod()
            if event.object.button == 1 then
                color[1] = color[1] * 1.25
                object.Texture.setColorMod(color)
                -- 记录按压状态
                GlobalContext.IsMousePressed[object.getID()] = true
            end
            if event.object.button == 2 then
                color[2] = color[2] * 1.25
                object.Texture.setColorMod(color)
            end
            if event.object.button == 3 then
                color[3] = color[3] * 1.25
                object.Texture.setColorMod(color)
            end
        end
        -- 释放鼠标按键
        if event.type == EventType.Object.MouseRelease then
            local color = object.Texture.getColorMod()
            if event.object.button == 1 then
                color[1] = color[1] / 1.25
                object.Texture.setColorMod(color)
                --- 记录释放状态
                GlobalContext.IsMousePressed[object.getID()] = false
            end
            if event.object.button == 2 then
                color[2] = color[2] / 1.25
                object.Texture.setColorMod(color)
            end
            if event.object.button == 3 then
                color[3] = color[3] / 1.25
                object.Texture.setColorMod(color)
            end
        end
    end
end

Event.registerCallback(Self, MouseHandle)
