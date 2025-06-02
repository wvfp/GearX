--- 这个脚本附加到背景中，主要用于初始化游戏的全局变量和设置。
--- 该脚本在游戏开始时运行一次。

-- 资产路径
AssetPath = getCurrentDir() .. "/"
-- 音效配置
SoundConfig = {
    -- 背景音乐
    BackGroundMusic = {
        -- 背景音乐路径
        Path = AssetPath .. "Sounds/background.mp3",
        -- 音量
        Volume = 0.7,
        channel = nil
    },
    -- 点击音效
    ClickSound = {
        -- 点击音效路径
        Path = AssetPath .. "Sounds/click.wav",
        -- 音量
        Volume = 0.8,
        channel = nil
    },
    -- 步骤音效
    StepSound = {
        -- 步骤音效路径
        Path = AssetPath .. "Sounds/step.wav",
        -- 音量
        Volume = 0.3,
        channel = nil
    }
}
-- 播放背景音乐
Audio.loadMusicFromPath(SoundConfig.BackGroundMusic.Path)
Audio.setMusicVolume(SoundConfig.BackGroundMusic.Volume)
Audio.playMusic()

function playClickChunk()
    Audio.playChunk(SoundConfig.ClickSound.Path)
    --- 先播放后设置音量
    Audio.setChunkVolume(SoundConfig.ClickSound.Path, SoundConfig.ClickSound.Volume)
end

function playStepChunk()
    Audio.playChunk(SoundConfig.StepSound.Path)
    --- 先播放后设置音量
    Audio.setChunkVolume(SoundConfig.StepSound.Path, SoundConfig.StepSound.Volume)
end
