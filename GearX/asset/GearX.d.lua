---@meta

---@class GKeyEnum
---@field A integer
---@field B integer
---@field C integer
---@field D integer
---@field E integer
---@field F integer
---@field G integer
---@field H integer
---@field I integer
---@field J integer
---@field K integer
---@field L integer
---@field M integer
---@field N integer
---@field O integer
---@field P integer
---@field Q integer
---@field R integer
---@field S integer
---@field T integer
---@field U integer
---@field V integer
---@field W integer
---@field X integer
---@field Y integer
---@field Z integer
---@field Zero integer
---@field One integer
---@field Two integer
---@field Three integer
---@field Four integer
---@field Five integer
---@field Six integer
---@field Seven integer
---@field Eight integer
---@field Nine integer
---@field F1 integer
---@field F2 integer
---@field F3 integer
---@field F4 integer
---@field F5 integer
---@field F6 integer
---@field F7 integer
---@field F8 integer
---@field F9 integer
---@field F10 integer
---@field F11 integer
---@field F12 integer
---@field Escape integer
---@field LShift integer
---@field RShift integer
---@field LControl integer
---@field RControl integer
---@field LAlt integer
---@field RAlt integer
---@field Space integer
---@field Enter integer
---@field Backspace integer
---@field Up integer
---@field Down integer
---@field Left integer
---@field Right integer
---@field Insert integer
---@field Delete integer
---@field Home integer
---@field End integer
---@field PageUp integer
---@field PageDown integer
---@field Tab integer
---@field CapsLock integer
---@field NumLock integer
---@field ScrollLock integer
---@field Pause integer
---@field PrintScreen integer
---@field Menu integer
---@field LeftBracket integer
---@field RightBracket integer
---@field Semicolon integer
---@field Comma integer
---@field Period integer


---@class TransformComponent
---@field getPosition fun(): table 获取世界坐标(不同于引擎中的位置(物体右上角坐标),这是物体几何中心的坐标
---@field setPosition fun( pos: table) 设置世界坐标(不同于引擎中的位置(物体右上角坐标),这是物体几何中心的坐标
---@field addPositionXOffset fun( dx: number) X轴相对移动
---@field addPositionYOffset fun( dy: number) Y轴相对移动
---@field getAngle fun(): number 获取旋转角度（角度制）
---@field setAngle fun( degrees: number) 设置绝对角度
---@field addAngleOffset fun( delta: number) 相对旋转角度
---@field getScale fun(): table 获取缩放比例
---@field setScale fun( scale: table) 设置整体缩放
---@field setScaleX fun( sx: number) 设置X轴单独缩放
---@field setScaleY fun( sy: number) 设置Y轴单独缩放
---@field addScaleOffset fun( delta: table) 叠加缩放比例

---@class TextureComponent
---@field getTexturePath fun(): string 获取纹理文件路径
---@field loadTextureFromPath fun( path: string): boolean 异步加载新纹理
---@field getTextureRect fun(): table 获取当前显示区域
---@field setTextureRect fun( rect: table) 设置显示区域（裁切）
---@field getTextureWidth fun(): integer 获取原始纹理宽度
---@field getTextureHeight fun(): integer 获取原始纹理高度
---@field getTextureSize fun(): table 获取当前显示尺寸
---@field getColorMod fun(): table 获取颜色调制参数
---@field setColorMod fun( color: table) 设置颜色调制
---@field getScaleMod fun(): ScaleMode 获取缩放过滤模式
---@field setScaleMod fun( mode: ScaleMode) 设置缩放过滤模式
---@field getBlendMode fun(): BlendMode 获取混合模式
---@field setBlendMode fun( mode: BlendMode) 设置混合模式
---@field getFlipMode fun(): FlipMode 获取翻转状态
---@field setFlipMode fun( mode: FlipMode) 设置翻转状态
---@field getShape fun(): Shape 获取碰撞体形状

---@class RigidBodyComponent
---@field getVelocity fun(): table 获取线速度
---@field setVelocity fun( vel: table) 设置绝对速度
---@field addVelocityOffset fun( delta: table) 叠加速度
---@field addVelocityXOffset fun( dx: number) X轴叠加速度
---@field addVelocityYOffset fun( dy: number) Y轴叠加速度
---@field getAngularVelocity fun(): number 获取角速度（度/秒）
---@field setAngularVelocity fun( speed: number) 设置绝对角速度
---@field addAngularVelocityOffset fun( delta: number) 叠加角速度
---@field getGravityScale fun(): number 获取重力系数
---@field setGravityScale fun( scale: number) 设置重力系数
---@field getFriction fun(): number  获取摩擦系数 0-1
---@field setFriction fun( value: number) 设置摩擦系数 0-1
---@field getRestitution fun(): number  获取弹性系数 0-1
---@field setRestitution fun( value: number) 设置弹性系数 0-1
---@field getDensity fun(): number 获取密度
---@field setDensity fun( value: number) 设置材料密度
---@field isSensor fun(): boolean 是否为传感器（不产生物理反馈）
---@field setSensor fun( value: boolean) 设置传感器模式
---@field getRestitutionThreshold fun(): number 获取反弹阈值
---@field setRestitutionThreshold fun( value: number) 设置反弹阈值
---@field getFixedRotation fun(): boolean 是否固定旋转
---@field setFixedRotation fun( value: boolean) 设置旋转锁定
---@field applyForce fun( force: table) 施加持续力
---@field applyForceToCenter fun( force: table) 向质心施加力
---@field applyTorque fun( torque: number) 施加扭矩
---@field applyImpulse fun( impulse: table) 施加瞬时冲量
---@field applyImpulseToCenter fun( impulse: table) 向质心施加冲量
---@field applyAngularImpulse fun( impulse: number) 施加角冲量

---@class GObject
---@field isValid fun(): boolean 对象是否有效
---@field isValidTexture fun(): boolean 纹理组件是否有效
---@field isValidRigidBody fun(): boolean 刚体组件是否有效
---@field isValidTransform fun(): boolean 变换组件是否有效
---@field getID fun(): number 获取对象唯一标识符
---@field getName fun(): string 获取对象名称
---@field setName fun( name: string) 设置对象名称
---@field getLayer fun(): number 获取对象所在层级
---@field setLayer fun( layer: number) 设置对象所在层级
---@field getVisible fun(): boolean 获取对象可见性
---@field setVisible fun( visible: boolean) 设置对象可见性
---@field Transform TransformComponent 变换组件访问器
---@field Texture TextureComponent 纹理组件访问器
---@field RigidBody RigidBodyComponent 刚体组件访问器

---@class GLevel
---@field getName fun(): string 获取当前关卡名称
---@field setName fun( name: string) 设置当前关卡名称
---@field getGravity fun(): table 获取全局重力向量
---@field setGravity fun( gravity: table) 设置全局重力向量
---@field getRenderRect fun(): table 获取渲染区域（视口）
---@field setRenderRect fun( rect: table) 设置渲染区域（视口）
---@field getTargetTextureSize fun(): table 获取渲染纹理尺寸
---@field setTargetTextureSize fun( size: table) 设置渲染纹理尺寸
---@field createObject fun( name: string): GObject 创建新对象并返回其ID

---@class GAudio
---@field getMusicVolume fun(): number 获取音乐音量（0-1）
---@field setMusicVolume fun( volume: number) 设置音乐音量（0-1）
---@field getMusicLoopStartTime fun() 获取循环开始时间
---@field getMusicLoopEndTime fun() 获取循环结束时间
---@field getMusicLoopLengthTime fun() 获取循环时长
---@field getMusicPosition fun(): number 获取音乐播放位置（秒）
---@field setMusicPosition fun( position: number) 设置音乐播放位置（秒）
---@field getChunkVolume fun(path:string): number 获取音效音量（0-1）
---@field setChunkVolume fun(path:string, volume: number) 设置音效音量（0-1）
---@field playMusic fun() 播放音乐（异步加载）
---@field stopMusic fun() 停止音乐
---@field pauseMusic fun() 暂停音乐
---@field resumeMusic fun() 恢复音乐播放
---@field playChunk fun( path: string) 播放音效（异步加载）
---@field stopChunk fun( path: string) 停止音效
---@field pauseChunk fun( path: string) 暂停音效
---@field resumeChunk fun( path: string) 恢复音效播放
---@field loadMusicFromPath fun( path: string) 加载音乐文件
---@field unloadMusic fun() 卸载当前音乐

---鼠标事件类型
---@class MouseEventType
---@field ButtonDown integer 鼠标按键按下事件
---@field ButtonUp integer 鼠标按键松开事件
---@field Motion integer 鼠标移动事件
---@field Wheel integer 鼠标滚轮事件

--- 键盘事件类型
---@class KeyboardEventType
---@field KeyMapChanged integer 键盘映射改变事件
---@field KeyDown integer 键盘按键按下事件
---@field KeyUp integer 键盘按键松开事件

---@class GEventType 事件类型
---@field Mouse MouseEventType 鼠标事件类型
---@field Keyboard KeyboardEventType 键盘事件类型

---鼠标事件
---@class MouseEvent
---@field button integer 鼠标按钮（SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT等）
---@field x number 鼠标X坐标
---@field y number 鼠标Y坐标
---@field clicks number 鼠标点击次数
---@field timestamp number 事件时间戳
---@field xrel number 鼠标X轴相对移动
---@field yrel number 鼠标Y轴相对移动
---@field state integer 鼠标按键状态（SDL_BUTTON_LMASK等）
---@field direction number 鼠标滚轮方向（SDL_MOUSEWHEEL_NORMAL, SDL_MOUSEWHEEL_FLIPPED等）


---键盘事件
---@class KeyboardEvent
---@field keysym integer 键盘按键符号（SDLK_a, SDLK_b等）
---@field scancode integer 键盘扫描码（SDL_SCANCODE_A, SDL_SCANCODE_B等）
---@field repeat boolean 是否为重复按键事件
---@field timestamp number 事件时间戳
---@field Key_ GKeyEnum 键盘按键枚举


---@class GEvent
---@field type integer 事件类型
---@field mouse MouseEvent 鼠标事件数据
---@field keyboard KeyboardEvent 键盘事件数据
---纹理缩放模式枚举
---@alias ScaleMode
---| "Linear"   # 线性插值（平滑缩放）
---| "Nearest"  # 最近邻插值（像素风效果）

---渲染混合模式枚举
---@alias BlendMode
---| "None"               # 无混合（完全覆盖）
---| "Add"                # 加法混合（光效增强）
---| "AddPremultiplied"   # 预乘加法混合（带Alpha处理）
---| "Blend"              # 标准Alpha混合（半透明效果）
---| "BlendPremultiplied" # 预乘Alpha混合（性能优化）
---| "Mod"                # 取模混合（特殊滤镜效果）

---对象翻转模式枚举
---@alias FlipMode
---| "Horizontal"           # 水平翻转
---| "Vertical"             # 垂直翻转
---| "VerticalAndHorizontal" # 水平垂直同时翻转

---碰撞体形状类型枚举
---@alias ShapeType
---| "Rect"    # 矩形碰撞体
---| "Circle"  # 圆形碰撞体

---碰撞体形状定义
---@alias Shape
---| "Rect"
---| "Circle"

---通过ID获取游戏对象
---@param id number 对象唯一标识符（可在编辑器中查看）
---@return GObject | nil 找不到时返回nil
---@usage local obj = getObject(1001)
---@usage if obj and obj:isValid() then obj:setPosition(100,200) end
function getObject(id) end

---获取场景中所有有效对象
---@return table<number, GObject> 以ID为key的对象表
---@usage for id, obj in pairs(getAllObject()) do print(obj:getID()) end
function getAllObject() end
---获取当前文件夹路径 World 所在目录
---@return string 当前路径
function getCurrentDir() end

---当前脚本所属的宿主对象（特殊全局变量）
---@global
---@type GObject
Self = nil

---当前关卡对象（特殊全局变量）
---@global
---@type GLevel
Level = nil
---游戏音频管理器（特殊全局变量）
---@global
---@type GAudio
Audio = nil

---DeltaTime（上一帧到当前帧的时间间隔，单位：秒）
---@global
---@type number
DeltaTime = 0

---当前产生碰撞的对象A（特殊全局变量）
---@Global
---@type number
ContactedObjectA = 0
---当前产生碰撞的对象B（特殊全局变量）
---@Global
---@type number
ContactedObjectB = 0

---全局KeyState（特殊全局变量）对应值键盘按键状态
---@Global
---@type table<integer, boolean>
KeyState = {};

---全局KeyStateSize（特殊全局变量）KeyState大小
---@Global
---@type integer
KeyStateSize = 0;

---类型枚举
---@global
---@type GEventType
EventType = nil;

---全局事件（特殊全局变量）
---@global
---@type GEvent
Event = nil;

---全局GlobalContext,用于实现类似c/c++的静态变量
---同时也可用于存储全局状态，方便跨lua文件使用
GlobalContext = {};
