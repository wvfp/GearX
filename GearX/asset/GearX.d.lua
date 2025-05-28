---@meta

---@class Vec2
---@field x number
---@field y number

---@class Rect
---@field x number
---@field y number
---@field w number
---@field h number

---@class Color
---@field r number [0-1]
---@field g number [0-1]
---@field b number [0-1]
---@field a number [0-1]

---@class TransformComponent
---@field getPosition fun(self): Vec2 获取世界坐标(不同于引擎中的位置(物体右上角坐标),这是物体几何中心的坐标
---@field setPosition fun(self, pos: Vec2) 设置世界坐标(不同于引擎中的位置(物体右上角坐标),这是物体几何中心的坐标
---@field addPositionXOffset fun(self, dx: number) X轴相对移动
---@field addPositionYOffset fun(self, dy: number) Y轴相对移动
---@field getAngle fun(self): number 获取旋转角度（角度制）
---@field setAngle fun(self, degrees: number) 设置绝对角度
---@field addAngleOffset fun(self, delta: number) 相对旋转角度
---@field getScale fun(self): Vec2 获取缩放比例
---@field setScale fun(self, scale: Vec2) 设置整体缩放
---@field setScaleX fun(self, sx: number) 设置X轴单独缩放
---@field setScaleY fun(self, sy: number) 设置Y轴单独缩放
---@field addScaleOffset fun(self, delta: Vec2) 叠加缩放比例

---@class TextureComponent
---@field getTexturePath fun(self): string 获取纹理文件路径
---@field loadTextureFromPath fun(self, path: string): boolean 异步加载新纹理
---@field getTextureRect fun(self): Rect 获取当前显示区域
---@field setTextureRect fun(self, rect: Rect) 设置显示区域（裁切）
---@field getTextureWidth fun(self): integer 获取原始纹理宽度
---@field getTextureHeight fun(self): integer 获取原始纹理高度
---@field getTextureSize fun(self): Vec2 获取当前显示尺寸
---@field getColorMod fun(self): Color 获取颜色调制参数
---@field setColorMod fun(self, color: Color) 设置颜色调制
---@field getScaleMod fun(self): ScaleMode 获取缩放过滤模式
---@field setScaleMod fun(self, mode: ScaleMode) 设置缩放过滤模式
---@field getBlendMode fun(self): BlendMode 获取混合模式
---@field setBlendMode fun(self, mode: BlendMode) 设置混合模式
---@field getFlipMode fun(self): FlipMode 获取翻转状态
---@field setFlipMode fun(self, mode: FlipMode) 设置翻转状态
---@field getShape fun(self): Shape 获取碰撞体形状

---@class RigidBodyComponent
---@field getVelocity fun(self): Vec2 获取线速度
---@field setVelocity fun(self, vel: Vec2) 设置绝对速度
---@field addVelocityOffset fun(self, delta: Vec2) 叠加速度
---@field addVelocityXOffset fun(self, dx: number) X轴叠加速度
---@field addVelocityYOffset fun(self, dy: number) Y轴叠加速度
---@field getAngularVelocity fun(self): number 获取角速度（度/秒）
---@field setAngularVelocity fun(self, speed: number) 设置绝对角速度
---@field addAngularVelocityOffset fun(self, delta: number) 叠加角速度
---@field getGravityScale fun(self): number 获取重力系数
---@field setGravityScale fun(self, scale: number) 设置重力系数
---@field getFriction fun(self): number  获取摩擦系数 0-1
---@field setFriction fun(self, value: number) 设置摩擦系数 0-1
---@field getRestitution fun(self): number  获取弹性系数 0-1
---@field setRestitution fun(self, value: number) 设置弹性系数 0-1
---@field getDensity fun(self): number 获取密度
---@field setDensity fun(self, value: number) 设置材料密度
---@field isSensor fun(self): boolean 是否为传感器（不产生物理反馈）
---@field setSensor fun(self, value: boolean) 设置传感器模式
---@field getRestitutionThreshold fun(self): number 获取反弹阈值
---@field setRestitutionThreshold fun(self, value: number) 设置反弹阈值
---@field getFixedRotation fun(self): boolean 是否固定旋转
---@field setFixedRotation fun(self, value: boolean) 设置旋转锁定
---@field applyForce fun(self, force: Vec2) 施加持续力
---@field applyForceToCenter fun(self, force: Vec2) 向质心施加力
---@field applyTorque fun(self, torque: number) 施加扭矩
---@field applyImpulse fun(self, impulse: Vec2) 施加瞬时冲量
---@field applyImpulseToCenter fun(self, impulse: Vec2) 向质心施加冲量
---@field applyAngularImpulse fun(self, impulse: number) 施加角冲量

---@class GObject
---@field isValid fun(self): boolean 对象是否有效
---@field isValidTexture fun(self): boolean 纹理组件是否有效
---@field isValidRigidBody fun(self): boolean 刚体组件是否有效
---@field isValidTransform fun(self): boolean 变换组件是否有效
---@field getID fun(self): number 获取对象唯一标识符
---@field getName fun(self): string 获取对象名称
---@field setName fun(self, name: string) 设置对象名称
---@field getLayer fun(self): number 获取对象所在层级
---@field setLayer fun(self, layer: number) 设置对象所在层级
---@field getVisible fun(self): boolean 获取对象可见性
---@field setVisible fun(self, visible: boolean) 设置对象可见性
---@field Transform TransformComponent 变换组件访问器
---@field Texture TextureComponent 纹理组件访问器
---@field RigidBody RigidBodyComponent 刚体组件访问器

---@class GLevel
---@field getName fun(self): string 获取当前关卡名称
---@field setName fun(self, name: string) 设置当前关卡名称
---@field getGravity fun(self): Vec2 获取全局重力向量
---@field setGravity fun(self, gravity: Vec2) 设置全局重力向量
---@field getRenderRect fun(self): Rect 获取渲染区域（视口）
---@field setRenderRect fun(self, rect: Rect) 设置渲染区域（视口）
---@field getTargetTextureSize fun(self): Vec2 获取渲染纹理尺寸
---@field setTargetTextureSize fun(self, size: Vec2) 设置渲染纹理尺寸
---@field createObject fun(self, name: string): GObject 创建新对象并返回其ID

---@class GAudio
---@field getMusicVolume fun(self): number 获取音乐音量（0-1）
---@field setMusicVolume fun(self, volume: number) 设置音乐音量（0-1）
---@field getMusicLoopStartTime fun(self) 获取循环开始时间
---@field getMusicLoopEndTime fun(self) 获取循环结束时间
---@field getMusicLoopLengthTime fun(self) 获取循环时长
---@field getMusicPosition fun(self): number 获取音乐播放位置（秒）
---@field setMusicPosition fun(self, position: number) 设置音乐播放位置（秒）
---@field getChunkVolume fun(self,path:string): number 获取音效音量（0-1）
---@field setChunkVolume fun(self,payh:string, volume: number) 设置音效音量（0-1）
---@field playMusic fun(self) 播放音乐（异步加载）
---@field stopMusic fun(self) 停止音乐
---@field pauseMusic fun(self) 暂停音乐
---@field resumeMusic fun(self) 恢复音乐播放
---@field playChunk fun(self, path: string) 播放音效（异步加载）
---@field stopChunk fun(self, path: string) 停止音效
---@field pauseChunk fun(self, path: string) 暂停音效
---@field resumeChunk fun(self, path: string) 恢复音效播放
---@field loadMusicFromPath fun(self, path: string) 加载音乐文件
---@field unloadMusic fun(self) 卸载当前音乐

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
---@field state number 鼠标按键状态（SDL_BUTTON_LMASK等）
---@field direction number 鼠标滚轮方向（SDL_MOUSEWHEEL_NORMAL, SDL_MOUSEWHEEL_FLIPPED等）


---键盘事件
---@class KeyboardEvent
---@field keysym number 键盘按键符号（SDLK_a, SDLK_b等）
---@field scancode number 键盘扫描码（SDL_SCANCODE_A, SDL_SCANCODE_B等）
---@field repeat boolean 是否为重复按键事件
---@field timestamp number 事件时间戳


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

