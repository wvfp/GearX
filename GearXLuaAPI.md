# GearX 游戏引擎 Lua API 文档

本文件详细介绍 GearX 游戏引擎对 Lua 脚本开放的 API，包括主要类、方法、事件、全局变量与工具函数,示例游戏在`GearX\example\`目录下

---

## 目录

1. [按键枚举](#按键枚举)
2. [核心组件](#核心组件)
   - [TransformComponent（变换组件）](#transformcomponent变换组件)
   - [TextureComponent（纹理组件）](#texturecomponent纹理组件)
   - [RigidBodyComponent（刚体组件）](#rigidbodycomponent刚体组件)
3. [游戏对象与关卡](#游戏对象与关卡)
   - [GObject（游戏对象）](#gobject游戏对象)
   - [GLevel（关卡）](#glevel关卡)
4. [音频管理](#音频管理)
   - [GAudio（音频）](#gaudio音频)
5. [事件系统](#事件系统)
   - [鼠标事件类型](#鼠标事件类型)
   - [键盘事件类型](#键盘事件类型)
   - [游戏对象事件类型](#游戏对象事件类型)
   - [GEvent（事件）](#gevent事件)
6. [全局常量与变量](#全局常量与变量)
   - [缩放模式（ScaleMode）](#缩放模式scalemode)
   - [混合模式（BlendMode）](#混合模式blendmode)
   - [翻转模式（FlipMode）](#翻转模式flipmode)
   - [形状类型（ShapeType）](#形状类型shapetype)
   - [工具函数](#工具函数)
   - [全局变量](#全局变量)

---

## 按键枚举

### `GKeyEnum`
定义了所有支持的键盘按键：

```
A-Z, Zero-Nine, F1-F12,
Escape, LShift, RShift, LControl, RControl, LAlt, RAlt,
Space, Enter, Backspace, Up, Down, Left, Right,
Insert, Delete, Home, End, PageUp, PageDown,
Tab, CapsLock, NumLock, ScrollLock, Pause,
PrintScreen, Menu, LeftBracket, RightBracket,
Semicolon, Comma, Period
```

---

## 核心组件

### TransformComponent（变换组件）

处理游戏对象的位置、旋转和缩放。

#### 方法

| 方法名 | 描述 |
|--------|------|
| `getPosition()` | 获取世界坐标（物体几何中心） |
| [`setPosition(pos)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置世界坐标 |
| [`addPositionXOffset(dx)`](./GearX/runtime/core/utils/object_wrapper.hpp) | X轴相对移动 |
| [`addPositionYOffset(dy)`](./GearX/runtime/core/utils/object_wrapper.hpp) | Y轴相对移动 |
| [`getAngle()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取旋转角度（角度制） |
| [`setAngle(degrees)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置绝对角度 |
| [`addAngleOffset(delta)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 相对旋转角度 |
| [`getScale()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取缩放比例 |
| [`setScale(scale)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置整体缩放 |
| [`setScaleX(sx)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置X轴单独缩放 |
| [`setScaleY(sy)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置Y轴单独缩放 |
| [`addScaleOffset(delta)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 叠加缩放比例 |

---

### TextureComponent（纹理组件）

处理游戏对象的纹理属性。

#### 方法

| 方法名 | 描述 |
|--------|------|
| [`getTexturePath()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取纹理文件路径 |
| [`loadTextureFromPath(path)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 异步加载新纹理 |
| [`getTextureRect()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取当前显示区域 |
| [`setTextureRect(rect)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置显示区域（裁切） |
| [`getTextureWidth()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取原始纹理宽度 |
| [`getTextureHeight()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取原始纹理高度 |
| [`getTextureSize()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取纹理尺寸 |
| [`getSize()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取对象长宽 |
| [`setSize()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置对象长宽 |
| [`getColorMod()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取颜色调制参数 `{r: (0-1), g: (0-1), b: (0-1), a: (0-1)}` |
| [`setColorMod(color)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置颜色调制 |
| [`getScaleMod()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取缩放过滤模式 |
| [`setScaleMod(mode)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置缩放过滤模式 |
| [`getBlendMode()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取混合模式 |
| [`setBlendMode(mode)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置混合模式 |
| [`getFlipMode()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取翻转状态 |
| [`setFlipMode(mode)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置翻转状态 |
| [`getShape()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取碰撞体形状 |

---

### RigidBodyComponent（刚体组件）

处理游戏对象的物理行为。

#### 方法

| 方法名 | 描述 |
|--------|------|
| [`getVelocity()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取线速度 |
| [`setVelocity(vel)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置绝对速度 |
| [`addVelocityOffset(delta)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 叠加速度 |
| [`addVelocityXOffset(dx)`](./GearX/runtime/core/utils/object_wrapper.hpp) | X轴叠加速度 |
| [`addVelocityYOffset(dy)`](./GearX/runtime/core/utils/object_wrapper.hpp) | Y轴叠加速度 |
| [`getAngularVelocity()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取角速度（度/秒） |
| [`setAngularVelocity(speed)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置绝对角速度 |
| [`addAngularVelocityOffset(delta)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 叠加角速度 |
| [`getGravityScale()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取重力系数 |
| [`setGravityScale(scale)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置重力系数 |
| [`getFriction()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取摩擦系数 0-1 |
| [`setFriction(value)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置摩擦系数 0-1 |
| [`getRestitution()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取弹性系数 0-1 |
| [`setRestitution(value)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置弹性系数 0-1 |
| [`getDensity()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取密度 |
| [`setDensity(value)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置材料密度 |
| [`isSensor()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 是否为传感器 |
| [`setSensor(value)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置传感器模式 |
| [`getRestitutionThreshold()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取反弹阈值 |
| [`setRestitutionThreshold(value)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置反弹阈值 |
| [`getFixedRotation()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 是否固定旋转 |
| [`setFixedRotation(value)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置旋转锁定 |
| [`applyForce(force)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 施加持续力 |
| [`applyForceToCenter(force)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 向质心施加力 |
| [`applyTorque(torque)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 施加扭矩 |
| [`applyImpulse(impulse)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 施加瞬时冲量 |
| [`applyImpulseToCenter(impulse)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 向质心施加冲量 |
| [`applyAngularImpulse(impulse)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 施加角冲量 |

---

## 游戏对象与关卡

### GObject（游戏对象）

表示游戏中的基本实体。

#### 方法

| 方法名 | 描述 |
|--------|------|
| [`isValid()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 对象是否有效 |
| [`isValidTexture()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 纹理组件是否有效 |
| [`isValidRigidBody()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 刚体组件是否有效 |
| [`isValidTransform()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 变换组件是否有效 |
| [`getID()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取对象唯一标识符 |
| [`getName()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取对象名称 |
| [`setName(name)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置对象名称 |
| [`getLayer()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取对象所在层级 |
| [`setLayer(layer)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置对象所在层级 |
| `getVisible()` | 获取对象可见性 |
| [`setVisible(visible)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置对象可见性 |

#### 属性访问器

- `Transform` - 变换组件
- `Texture` - 纹理组件
- `RigidBody` - 刚体组件

---

### GLevel（关卡）

表示当前的游戏关卡。

#### 方法

| 方法名 | 描述 |
|--------|------|
| [`getName()`](./GearX/runtime/core/utils/object_wrapper.hpp) | 获取当前关卡名称 |
| [`setName(name)`](./GearX/runtime/core/utils/object_wrapper.hpp) | 设置当前关卡名称 |
| [`getGravity()`](./GearX/runtime/core/framework/level/level.hpp) | 获取全局重力向量 |
| [`setGravity(gravity)`](./GearX/runtime/core/framework/level/level.hpp) | 设置全局重力向量 |
| [`getRenderRect()`](./GearX/runtime/core/framework/level/level.hpp) | 获取渲染区域（视口） |
| [`setRenderRect(rect)`](./GearX/runtime/core/framework/level/level.hpp) | 设置渲染区域（视口） |
| [`getTargetTextureSize()`](./GearX/runtime/core/framework/level/level.hpp) | 获取渲染纹理尺寸 |
| [`setTargetTextureSize(size)`](./GearX/runtime/core/framework/level/level.hpp) | 设置渲染纹理尺寸 |
| `createObject(name)` | 创建新对象并返回其ID |

---

## 音频管理

### GAudio（音频）

负责管理游戏中的音频播放。

#### 方法

| 方法名 | 描述 |
|--------|------|
| `getMusicVolume()` | 获取音乐音量（0-1） |
| `setMusicVolume(volume)` | 设置音乐音量（0-1） |
| `getMusicLoopStartTime()` | 获取循环开始时间 |
| `getMusicLoopEndTime()` | 获取循环结束时间 |
| `getMusicLoopLengthTime()` | 获取循环时长 |
| `getMusicPosition()` | 获取音乐播放位置（秒） |
| `setMusicPosition(position)` | 设置音乐播放位置（秒） |
| `getChunkVolume(path)` | 获取音效音量（0-1） |
| `setChunkVolume(path, volume)` | 设置音效音量（0-1） |
| [`getChunkState(path)`](./GearX/runtime/core/system/audio/audio_system.hpp) | 获取音效状态 1:播放中 2:暂停 3:停止 |
| [`getChannel(path)`](./GearX/runtime/core/system/audio/audio_system.hpp) | 获取音效通道 |
| `playMusic()` | 播放音乐（异步加载） |
| `stopMusic()` | 停止音乐 |
| `pauseMusic()` | 暂停音乐 |
| `resumeMusic()` | 恢复音乐播放 |
| `playChunk(path)` | 播放音效（异步加载） |
| `stopChunk(path)` | 停止音效 |
| `pauseChunk(path)` | 暂停音效 |
| `resumeChunk(path)` | 恢复音效播放 |
| [`loadMusicFromPath(path)`](./GearX/runtime/core/system/audio/audio_system.hpp) | 加载音乐文件 |
| [`unloadMusic()`](./GearX/runtime/core/system/audio/audio_system.hpp) | 卸载当前音乐 |

---

## 事件系统

### 鼠标事件类型

| 事件 | 描述 |
|------|------|
| `ButtonDown` | 鼠标按键按下事件 |
| `ButtonUp` | 鼠标按键松开事件 |
| `Motion` | 鼠标移动事件 |
| `Wheel` | 鼠标滚轮事件 |

---

### 键盘事件类型

| 事件 | 描述 |
|------|------|
| `KeyMapChanged` | 键盘映射改变事件 |
| `KeyDown` | 键盘按键按下事件 |
| `KeyUp` | 键盘按键松开事件 |

---

### 游戏对象事件类型

| 事件 | 描述 |
|------|------|
| `MouseHover` | 鼠标悬停事件 |
| `MouseMove` | 鼠标移动事件 |
| `MouseEnter` | 鼠标进入事件 |
| `MouseLeave` | 鼠标离开事件 |
| `MouseClick` | 鼠标点击事件 |
| `MousePress` | 鼠标按下事件 |
| `MouseRelease` | 鼠标松开事件 |
| `MouseWheel` | 鼠标滚轮事件 |

---

### GEvent（事件）

封装了所有事件的基本信息。

#### 字段

| 字段 | 类型 | 描述 |
|------|------|------|
| `type` | integer | 事件类型 |
| `mouse` | MouseEvent | 鼠标事件数据 |
| `keyboard` | KeyboardEvent | 键盘事件数据 |
| `object` | GObjectEvent | 游戏对象事件数据 |

#### 方法

| 方法名 | 描述 |
|--------|------|
| `registerCallback(Object:GObject, func: function)` | 注册回调函数（func(GObject, GEvent)） |
| `unregisterCallback(Object:GObject, func: function)` | 注销回调函数 |
| `unregisterAllCallback()` | 注销所有回调函数 |
| `unregisterAllCallbackOfGObject(Object:GObject)` | 注销指定对象的所有回调函数 |

---

## 全局常量与变量

### 缩放模式（ScaleMode）

```lua
Linear       -- 线性缩放
Nearest      -- 最近邻缩放
Invalid      -- 不使用缩放模式
```

### 混合模式（BlendMode）

```lua
None                    -- 无混合
Add                     -- 加法混合
AddPremultiplied        -- 预乘加法混合
Blend                   -- 混合
BlendPremultiplied      -- 预乘混合
Mod                     -- 颜色调制混合
```

### 翻转模式（FlipMode）

```lua
None                -- 无翻转
Horizontal          -- 水平翻转
Vertical            -- 垂直翻转
VerticalAndHorizontal -- 水平和垂直翻转
```

### 形状类型（ShapeType）

```lua
Rect    -- 矩形形状
Circle  -- 圆形形状
```

### 工具函数

| 函数 | 描述 |
|------|------|
| `getObject(id)` | 通过ID获取游戏对象，找不到时返回nil |
| `getAllObject()` | 获取场景中所有有效对象 |
| `getCurrentDir()` | 获取当前文件夹路径 |

### 全局变量

| 变量 | 类型 | 描述 |
|------|------|------|
| `Self` | `GObject` | 当前脚本所属的宿主对象 |
| `Level` | `GLevel` | 当前关卡对象 |
| `Audio` | `GAudio` | 游戏音频管理器 |
| `DeltaTime` | `number` | 上一帧到当前帧的时间间隔（秒） |
| `ContactedObjectA` | `number` | 当前产生碰撞的对象A |
| `ContactedObjectB` | `number` | 当前产生碰撞的对象B |
| `KeyState` | `table<integer, boolean>` | 全局键盘按键状态 |
| `KeyStateSize` | `integer` | KeyState大小 |
| `EventType` | `GEventType` | 类型枚举 |
| `Event` | `GEvent` | 全局事件 |
| `GlobalContext` | `table` | 用于实现类似C/C++的静态变量 |

---

> 本文档由 `GearX.d.lua` 自动生成，建议配合源码和注释查阅。