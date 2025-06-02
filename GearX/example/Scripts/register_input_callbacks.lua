---@diagnostic disable: unnecessary-if
-- Controller for the Player Object
-- 资产路径
AssetPath = getCurrentDir() .. "/"

-- Play animation
-- 一个动画局部变量 index
-- 0 : Idle
-- 1 : Walk
-- 2 : Run
-- 3 : Jump
-- 4 : Attack
-- 5 : Die
-- 6 : Hurt
---@global AnimState
---@type table
AnimState = {
    Idle = 1,
    Run = 2,
    Jump = 3,
    Attack1 = 4,
    Attack2 = 5,
    Attack3 = 6,
    Die = 7,
    Hurt = 8,
    Fall = 9,
}
--- 配置:角色的移动和跳跃等参数
---@global PlayerConfig
---@type table
PlayerConfig = {
    MoveImpulse = 0.5,
    MaxSpeed = 4.0,
    JumpImpulse = 1.5
}

-- 初始化全局动画索引
if GlobalContext.AnimIndex == nil
then
    ---@global GlobalContext.AnimIndex
    ---@type number
    GlobalContext.AnimIndex = AnimState.Idle
end

local AnimIndex = GlobalContext.AnimIndex

---@type table
local AnimIdle = {
    size = 4,
    currentFrame = 1,
    AnimFrameTick = 7,
    -- 这里的路径需要根据实际资源位置调整
    paths = {
        "textures/sprite/Adventurer/Individual Sprites/adventurer-idle-00.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-idle-01.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-idle-02.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-idle-03.png",
    }
}
---@type table
local AnimRun = {
    size = 6,
    currentFrame = 1,
    AnimFrameTick = 5,
    paths = {
        "textures/sprite/Adventurer/Individual Sprites/adventurer-run-00.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-run-01.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-run-02.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-run-03.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-run-04.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-run-05.png"
    }
}
---@type table
local AnimJump = {
    size = 4,
    currentFrame = 1,
    AnimFrameTick = 4,
    paths = {
        "textures/sprite/Adventurer/Individual Sprites/adventurer-jump-00.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-jump-01.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-jump-02.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-jump-03.png",
    }
}
---@type table
local AnimAttack1 = {
    size = 5,
    currentFrame = 1,
    AnimFrameTick = 4,
    paths = {
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack1-00.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack1-01.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack1-02.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack1-03.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack1-04.png",
    }
}
---@type table
local AnimAttack2 = {
    size = 6,
    currentFrame = 1,
    AnimFrameTick = 4,
    paths = {
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack2-00.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack2-01.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack2-02.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack2-03.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack2-04.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack2-05.png",
    }
}
---@type table
local AnimAttack3 = {
    size = 5,
    currentFrame = 1,
    AnimFrameTick = 4,
    paths = {
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack3-00.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack3-01.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack3-02.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack3-03.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-attack3-04.png",
    }
}
---@type table
local AnimDie = {
    size = 7,
    currentFrame = 1,
    AnimFrameTick = 4,
    paths = {
        "textures/sprite/Adventurer/Individual Sprites/adventurer-die-00.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-die-01.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-die-02.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-die-03.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-die-04.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-die-05.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-die-06.png",
    }
}
---@type table
local AnimHurt = {
    size = 3,
    currentFrame = 1,
    AnimFrameTick = 4,
    paths = {
        "textures/sprite/Adventurer/Individual Sprites/adventurer-hurt-00.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-hurt-01.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-hurt-02.png",
    }
}
---@type table
local AnimFall = {
    size = 2,
    currentFrame = 1,
    AnimFrameTick = 4,
    paths = {
        "textures/sprite/Adventurer/Individual Sprites/adventurer-fall-00.png",
        "textures/sprite/Adventurer/Individual Sprites/adventurer-fall-01.png",
    }
}
---@global Direction
---@type table
-- 角色的朝向
Direction = {
    Left = 1,
    Right = 2,
}
-- 默认朝向为右
---@global direction
if direction == nil then
    direction = Direction.Right
end
if GlobalContext.AnimTable == nil then
    ---@global AnimTable
    GlobalContext.AnimTable = {
        AnimIdle,
        AnimRun,
        AnimJump,
        AnimAttack1,
        AnimAttack2,
        AnimAttack3,
        AnimDie,
        AnimHurt,
        AnimFall
    }
end

-- 角色行为
local function abs(x)
    if x < 0 then
        return -x
    end
    return x
end

local PlayerBehavior = {
    ---@param object GObject
    Move = function(object)
        AnimIndex = AnimState.Run
        if object.isValidRigidBody() then
            if direction == Direction.Left then
                object.RigidBody.applyImpulse({ -PlayerConfig.MoveImpulse, 0 });
            else
                object.RigidBody.applyImpulse({ PlayerConfig.MoveImpulse, 0 });
            end
            if abs(object.RigidBody.getVelocity()[1]) > PlayerConfig.MaxSpeed then
                if direction == Direction.Left then
                    object.RigidBody.setVelocity({ -PlayerConfig.MaxSpeed, 0 });
                else
                    object.RigidBody.setVelocity({ PlayerConfig.MaxSpeed, 0 });
                end
            end
        end
    end,
    ---@param object GObject
    Jump = function(object)
        -- AnimIndex = AnimState.Jump
        object.RigidBody.applyImpulse({ 0, -PlayerConfig.JumpImpulse })
    end,
    ---@param object GObject
    Attack1 = function(object)
        AnimIndex = AnimState.Attack1
        -- 这里可以添加攻击逻辑
    end,
    ---@param object GObject
    Attack2 = function(object)
        AnimIndex = AnimState.Attack2
        -- 这里可以添加攻击逻辑
    end,
    ---@param object GObject
    Attack3 = function(object)
        AnimIndex = AnimState.Attack3
        -- 这里可以添加攻击逻辑
    end,
    ---@param object GObject
    Die = function(object)
        AnimIndex = AnimState.Die
        -- 这里可以添加死亡逻辑
    end,
    ---@param object GObject
    Hurt = function(object)
        AnimIndex = AnimState.Hurt
        -- 这里可以添加受伤逻辑
    end,
}

---@param object GObject
---@param event GEvent
local function InputHandle(object, event)
    if event.type == EventType.Keyboard.KeyDown then
        if event.keyboard.keysym == event.keyboard.Key_.A then
            direction = Direction.Left
            PlayerBehavior.Move(object)
        elseif event.keyboard.keysym == event.keyboard.Key_.D then
            direction = Direction.Right
            PlayerBehavior.Move(object)
        elseif event.keyboard.keysym == event.keyboard.Key_.Space then
            PlayerBehavior.Jump(object)
        elseif event.keyboard.keysym == event.keyboard.Key_.J then
            PlayerBehavior.Attack1(object)
        elseif event.keyboard.keysym == event.keyboard.Key_.K then
            PlayerBehavior.Attack2(object)
        elseif event.keyboard.keysym == event.keyboard.Key_.L then
            PlayerBehavior.Attack3(object)
        end
    elseif event.type == EventType.Keyboard.KeyUp then
        -- 如果按键 A 或 D 松开，则切换到 Idle 状态
        if event.keyboard.keysym == event.keyboard.Key_.A or
            event.keyboard.keysym == event.keyboard.Key_.D then
            if AnimIndex == AnimState.Run then
                AnimIndex = AnimState.Idle
            end 
        end
    end
    GlobalContext.AnimIndex = AnimIndex
end



-- 注册键盘输入回调
Event.registerCallback(Self, InputHandle)
