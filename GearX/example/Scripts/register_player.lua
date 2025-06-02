-- 这个脚本执行一次，用于注册Player角色到全局作用域中
-- 便于其他对象的访问
-- 初始化全局Object索引
if GlobalContext.Objects == nil then
    GlobalContext.Objects = {}
end
---@global Player
---@type GObject
GlobalContext.Objects.Player = Self
