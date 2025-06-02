for k, v in pairs(GlobalContext.Objects.Ground) do
    if ContactedObjectA == v.getID() and ContactedObjectB == GlobalContext.Objects.Player.getID() or
        ContactedObjectA == GlobalContext.Objects.Player.getID() and ContactedObjectB == v.getID() then
        -- Player与Ground停止接触
        -- 设置全局变量，表示Player停止接触地面
        GlobalContext.PlayerContactGroundList[v.getID()] = false
        GlobalContext.PlayerOnGround = false
        -- 检查是否还有其他地面接触
        for i, v in pairs(GlobalContext.PlayerContactGroundList) do
            if v then
                GlobalContext.PlayerOnGround = true
                break
            end
        end
    end
end
