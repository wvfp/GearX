
for k, v in pairs(GlobalContext.Objects.Ground) do
    if ContactedObjectA == v.getID() and ContactedObjectB == GlobalContext.Objects.Player.getID() or
        ContactedObjectA == GlobalContext.Objects.Player.getID() and ContactedObjectB == v.getID() then
        -- Player与Ground接触
        -- 设置全局变量，表示Player接触地面
        GlobalContext.PlayerContactGroundList[v.getID()] = true
        GlobalContext.PlayerOnGround = true
    end
end
