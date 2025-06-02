---@diagnostic disable: unnecessary-if

---@type integer
local AnimIndex = GlobalContext.AnimIndex
local function PlayAnimation()
    AnimTable = GlobalContext.AnimTable;
    local anim = AnimTable[AnimIndex];
    if (anim == nil) then
        print("Animation not found for index: " .. AnimIndex)
        return
    end
    -- 更新当前帧
    if GlobalContext.AnimFrameTick == nil then
        GlobalContext.AnimFrameTick = 0
    end
    GlobalContext.AnimFrameTick = GlobalContext.AnimFrameTick + 1;
    if GlobalContext.AnimFrameTick < AnimTable[AnimIndex].AnimFrameTick then
        return -- 取消更新
    end
    -- 设置动画帧
    Self.Texture.loadTextureFromPath(AssetPath .. anim.paths[anim.currentFrame]);
    if (direction == Direction.Left) then
        -- 如果当前朝向是左，则水平翻转纹理
        Self.Texture.setFlipMode(FlipMode.Horizontal)
    else
        Self.Texture.setFlipMode(FlipMode.None)
    end
    GlobalContext.AnimFrameTick = 0 -- 重置计数器
    AnimTable[AnimIndex].currentFrame = AnimTable[AnimIndex].currentFrame + 1
    if (AnimTable[AnimIndex].currentFrame > AnimTable[AnimIndex].size) then
        if AnimIndex == AnimState.Jump then
            if GlobalContext.PlayerOnGround == false then
                -- 若角色在空中，则保持最后一帧
                AnimTable[AnimIndex].currentFrame = AnimTable[AnimIndex].size
            end
        else
            AnimTable[AnimIndex].currentFrame = 1 -- 重置为第一帧
        end
    end
    GlobalContext.AnimTable = AnimTable;
end

local function update()
    if GlobalContext.PlayerOnGround == false then
        local velocity = Self.RigidBody.getVelocity()
        if velocity[2] < 0 then
            -- AnimIndex = AnimState.Jump
        elseif velocity[2] == 0 then
            AnimIndex = AnimState.Idle
        elseif velocity[1] ~= 0 and velocity[2] > 0 then
            AnimIndex = AnimState.Run
        else
            AnimIndex = AnimState.Idle
        end
    elseif GlobalContext.PlayerOnGround == true then
        if AnimIndex ~= AnimState.Idle and AnimIndex ~= AnimState.Run then
            AnimIndex = AnimState.Idle
        end
    end
    if GlobalContext.PlayerOnGround == true and AnimIndex == AnimState.Run then
        if Audio.getChunkState(SoundConfig.StepSound.Path) == 3 then
            playStepChunk()
        end
    end
    GlobalContext.AnimIndex = AnimIndex
    PlayAnimation()
    GlobalContext.AnimIndex = AnimIndex
end

update()

