-- 这个脚本执行一次，用于注册Ground地面到全局作用域中
-- 便于其他对象的访问
-- 初始化全局Object索引
if GlobalContext.Objects == nil then
    GlobalContext.Objects = {}
    ---@global Ground
    ---@type table
    GlobalContext.Objects.Ground = {}
end
if GlobalContext.PlayerContactGroundList == nil then
    GlobalContext.PlayerContactGroundList = {}
end
--- 将Ground物体添加到Ground列表
table.insert(GlobalContext.Objects.Ground, Self)
