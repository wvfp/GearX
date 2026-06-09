# GearX — 完整模块化游戏引擎架构

> GearX 是一个类 Linux 内核设计的模块化 2D/3D 游戏引擎，内置 AI Agent 开发助手，支持 Windows/Linux/Android/Web 全平台，编辑器与运行时一体。

---

## 目录

1. [项目总览](#1-项目总览)
2. [目标平台与跨平台策略](#2-目标平台与跨平台策略)
3. [Cargo Workspace 结构](#3-cargo-workspace-结构)
4. [内核 (Kernel)](#4-内核-kernel)
5. [模块系统](#5-模块系统)
6. [AI Agent 系统](#6-ai-agent-系统)
7. [编辑器系统](#7-编辑器系统)
8. [运行时模块](#8-运行时模块)
9. [数据层](#9-数据层)
10. [构建与工作流](#10-构建与工作流)
11. [架构决策记录 (ADR)](#11-架构决策记录-adr)

---

## 1. 项目总览

### 1.1 核心理念

```
GearX = 微内核 + 可插拔模块 + 内置 AI Agent
```

- **内核最小化** — 只做模块管理、ECS、事件、任务调度、资源管理和基础服务
- **模块化** — 渲染、物理、音频、AI 等全是可插拔模块，通过统一 trait 接入
- **AI Agent 原生** — 开发助手是内核级模块，可访问引擎全部能力
- **编辑器+运行时一体** — 桌面版完整编辑器，Android 版覆盖层编辑器，Edit/Play 模式一键切换
- **全静态编译** — 所有模块静态链接，无 DLL/so 加载

### 1.2 引擎全景

```
┌──────────────────────────────────────────────────────────────────┐
│                    编辑器层 (Editor Layer)                        │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────────────┐   │
│  │ 场景视图  │ │Hierarchy │ │Inspector │ │  AI Agent 面板   │   │
│  ├──────────┤ ├──────────┤ ├──────────┤ ├──────────────────┤   │
│  │ 资源管理器 │ │ 控制台   │ │ 图编辑器  │ │  代码编辑器      │   │
│  └──────────┘ └──────────┘ └──────────┘ └──────────────────┘   │
├──────────────────────────────────────────────────────────────────┤
│                  AI Agent 系统 (内核级模块)                       │
│  ┌──────────────────────────────────────────────────────┐       │
│  │  LLM Runtime (可切换: ollama 本地 / OpenAI / Claude)   │       │
│  ├──────────────────────────────────────────────────────┤       │
│  │  Tool System: Scene | Code | Debug | Asset | Query    │       │
│  │  + Context Manager + Chat Interface                   │       │
│  └──────────────────────────────────────────────────────┘       │
├──────────────────────────────────────────────────────────────────┤
│                   运行时模块 (Runtime Modules)                     │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐           │
│  │ Render   │ │ Physics  │ │  Audio   │ │Animation │            │
│  │ (wgpu)   │ │(Rapier)  │ │ (kira)   │ │(库封装)  │            │
│  ├──────────┤ ├──────────┤ ├──────────┤ ├──────────┤           │
│  │ Particle │ │   AI     │ │   Nav    │ │   VFX    │            │
│  │ (GPU粒子) │ │行为树/FSM│ │ 寻路     │ │ 后处理   │            │
│  ├──────────┤ ├──────────┤ ├──────────┤ ├──────────┤           │
│  │  Input   │ │    UI    │ │Scripting │ │Network   │            │
│  │ (winit)  │ │  (egui)  │ │ (WASM)   │ │ 同步/RPC │            │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘           │
├──────────────────────────────────────────────────────────────────┤
│                       数据层 (Data Layer)                         │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────────────┐   │
│  │ 场景系统  │ │ Asset    │ │序列化    │ │ 文件系统 (VFS)   │   │
│  │ 预制体    │ │ Pipeline │ │ Scene    │ │ 热重载/Mod      │   │
│  └──────────┘ └──────────┘ └──────────┘ └──────────────────┘   │
├──────────────────────────────────────────────────────────────────┤
│                      内核 (Kernel Core)                          │
│  ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐ ┌────────────┐   │
│  │模块管理 │ │ ECS    │ │ 事件总线 │ │任务系统 │ │ 资源管理器  │   │
│  │(linkme) │ │bevy_ecs│ │ 混合IPC│ │并行调度 │ │ 缓存/生命周期│   │
│  ├────────┤ ├────────┤ ├────────┤ ├────────┤ ├────────────┤   │
│  │ 数学库  │ │ 内存   │ │Profiler │ │ Console │ │ 平台抽象    │   │
│  │ (glam) │ │分配器   │ │ 帧诊断  │ │ 日志    │ │ Platform   │   │
│  └────────┘ └────────┘ └────────┘ └────────┘ └────────────┘   │
├──────────────────────────────────────────────────────────────────┤
│                      平台层 (Platform)                            │
│  ┌────────────┐ ┌────────────┐ ┌──────────┐ ┌─────────────┐   │
│  │ Windows    │ │   Linux    │ │ Android  │ │ Web (WASM)  │   │
│  │ DX12/Vulkan│ │   Vulkan   │ │ Vulkan   │ │ WebGPU      │   │
│  └────────────┘ └────────────┘ └──────────┘ └─────────────┘   │
└──────────────────────────────────────────────────────────────────┘
```

### 1.3 关键设计决策

| 决策 | 选择 | 理由 |
|------|------|------|
| 语言 | Rust | 内存安全 + 零开销抽象 + 跨平台 |
| ECS | bevy_ecs standalone | 成熟稳定, Schedule 可复用做任务系统 |
| 渲染 | wgpu | DX12/Vulkan/Metal/WebGPU 统一 API |
| 物理 | Rapier | 纯 Rust, 2D+3D, 维护活跃 |
| 脚本 | WASM (wasmtime) | 跨平台, 多语言, 可 Android 动态加载 |
| 模块注册 | linkme | 零开销编译期静态注册 |
| 模块加载 | 全静态链接 | 简化架构, 支持 WASM, 无 ABI 问题 |
| AI Agent | 内核级模块 | 编辑器 + 运行时都可访问 |
| 编辑器 | 一体式 (Edit/Play 模式) | 桌面完整版, Android 覆盖层版 |
| IPC | 混合: 直接+trait+事件总线 | 按场景选最优通信方式 |
| 项目结构 | Cargo workspace | Rust 原生支持, 依赖清晰 |

---

## 2. 目标平台与跨平台策略

### 2.1 平台矩阵

```
┌─────────────────────────────────────────────────────────────────────┐
│ 平台         │ 开发  │ 运行  │ 编辑器 │ AI Agent │ 图形后端         │
├─────────────────────────────────────────────────────────────────────┤
│ Windows      │  ✅   │  ✅   │  ✅    │  ✅     │ DX12 / Vulkan    │
│ Linux        │  ✅   │  ✅   │  ✅    │  ✅     │ Vulkan           │
│ Android      │  ✅   │  ✅   │  ✅①   │  ✅②   │ Vulkan           │
│ Web (WASM)   │       │  ✅   │        │         │ WebGPU           │
└─────────────────────────────────────────────────────────────────────┘
① Android 编辑器为覆盖层(Overlay)模式
② Android AI Agent 需联网使用远程 LLM API
```

### 2.2 条件编译策略

```rust
// 平台检测宏 — 统一的条件编译入口

// 是否为桌面平台
#[cfg(any(target_os = "windows", target_os = "linux"))]
mod desktop;

// 是否为 Android
#[cfg(target_os = "android")]
mod android;

// 是否为 Web
#[cfg(target_arch = "wasm32")]
mod web;

// 是否有编辑器
#[cfg(feature = "editor")]   // 桌面 + Android 启用
mod editor;

// 是否有 AI Agent
#[cfg(feature = "ai_agent")] // 桌面 + Android 启用
mod ai_agent;
```

### 2.3 图形后端选择策略

| 平台 | 首选后端 | 回退后端 |
|------|---------|---------|
| Windows | DX12 | Vulkan |
| Linux | Vulkan | — |
| Android | Vulkan (Android 10+) | — |
| Web | WebGPU | WebGL 2.0 (fallback) |

### 2.4 平台抽象层

```rust
// kernel/src/platform/mod.rs

/// 平台抽象 — 所有平台相关操作通过此 trait 访问
pub trait Platform: Send + Sync + 'static {
    fn name(&self) -> &'static str;
    fn window_system(&self) -> Box<dyn WindowSystem>;
    fn file_system(&self) -> Box<dyn FileSystem>;
    fn time(&self) -> Box<dyn TimeSystem>;
    fn clipboard(&self) -> Box<dyn Clipboard>;
}

pub trait WindowSystem {
    fn create_window(&self, title: &str, w: u32, h: u32) -> Box<dyn Window>;
    fn run_event_loop(&mut self, callback: &mut dyn FnMut(PlatformEvent));
}

pub trait FileSystem {
    fn read(&self, path: &str) -> io::Result<Vec<u8>>;
    fn write(&self, path: &str, data: &[u8]) -> io::Result<()>;
    fn watch(&self, path: &str) -> Box<dyn FileWatcher>;
}

pub trait TimeSystem {
    fn now(&self) -> Duration;
    fn sleep(&self, dur: Duration);
}
```

### 2.5 各平台文件系统差异

```
Windows:  C:\Users\...\GearXProjects\
Linux:    ~/GearXProjects/
Android:  /storage/emulated/0/Android/data/com.gearx.engine/files/
Web:      IndexedDB (虚拟文件系统)
```

---

## 3. Cargo Workspace 结构

```toml
# /Cargo.toml
[workspace]
resolver = "2"
members = [
    "kernel",
    "modules/render",
    "modules/physics",
    "modules/audio",
    "modules/input",
    "modules/ui",
    "modules/scripting",
    "modules/animation",
    "modules/particle",
    "modules/ai_behavior",
    "modules/navigation",
    "modules/network",
    "modules/vfx",
    "editor",
    "ai_agent",
    "examples/sprite2d",
    "examples/minimal_3d",
    "examples/platformer",
]
```

```
gearx/
├── Cargo.toml                          # workspace root
├── LICENSE.txt
├── .gitignore
├── build/                              # 构建脚本、打包配置
│   ├── android/                        # Android Gradle + NDK 配置
│   ├── web/                            # WASM 打包脚本
│   └── wasm_compiler/                  # WASM 编译器服务
│
├── kernel/                             # 内核 (静态 lib)
│   ├── Cargo.toml
│   └── src/
│       ├── lib.rs                      # Kernel 主结构, 公开 API
│       ├── module/                     # 模块管理器
│       │   ├── mod.rs                  # Module trait, ModuleRegistry
│       │   └── registry.rs            # linkme 注册 + TypeMap 运行时
│       ├── ecs/                        # ECS 集成
│       │   ├── mod.rs                  # bevy_ecs re-export
│       │   ├── resources.rs           # 内核 Resource: Time, FrameStats
│       │   └── systems.rs             # 核心 ECS Systems
│       ├── event/                      # 事件总线
│       │   ├── mod.rs                  # EventBus
│       │   ├── channel.rs             # 跨模块通道
│       │   └── events.rs              # 预定义事件
│       ├── task/                       # 任务系统
│       │   ├── mod.rs                  # TaskSystem
│       │   └── executor.rs            # 并行执行器
│       ├── resource/                   # 资源管理器
│       │   ├── mod.rs                  # ResourceManager
│       │   ├── cache.rs               # 缓存策略
│       │   └── loader.rs              # 加载器接口
│       ├── math/                       # 数学 (glam re-export)
│       ├── platform/                   # 平台抽象
│       │   ├── mod.rs                  # Platform trait
│       │   ├── desktop.rs             # 桌面实现 (winit)
│       │   ├── android.rs             # Android 实现
│       │   └── web.rs                 # Web 实现
│       ├── profiler/                   # 性能分析
│       │   └── mod.rs                  # Frame Profiler
│       └── console/                    # 引擎控制台/日志
│           └── mod.rs                  # Console, Logger
│
├── modules/                            # 运行时模块
│   ├── render/                         # 渲染模块
│   │   ├── Cargo.toml
│   │   └── src/
│   │       ├── lib.rs                  # Module trait 实现
│   │       ├── render_graph.rs        # 渲染图
│   │       ├── pipeline.rs            # wgpu 管线
│   │       ├── camera.rs              # 相机系统
│   │       ├── mesh.rs                # 网格
│   │       ├── material.rs            # 材质系统
│   │       ├── light.rs               # 光照
│   │       ├── text.rs                # 文本
│   │       ├── postprocess.rs         # 后处理栈
│   │       └── backend.rs             # 后端选择
│   │
│   ├── physics/                        # 物理模块
│   │   ├── Cargo.toml
│   │   └── src/
│   │       ├── lib.rs
│   │       ├── world.rs               # 物理世界
│   │       ├── body.rs                # 刚体
│   │       ├── collider.rs            # 碰撞体
│   │       ├── joint.rs               # 约束
│   │       └── query.rs               # 形状/射线查询
│   │
│   ├── audio/                          # 音频模块
│   │   ├── Cargo.toml
│   │   └── src/
│   │       ├── lib.rs
│   │       ├── emitter.rs             # 音频发射器
│   │       ├── spatial.rs             # 3D 空间音频
│   │       ├── stream.rs              # 流式加载
│   │       └── mixer.rs               # 混音控制
│   │
│   ├── input/                          # 输入模块
│   │   ├── Cargo.toml
│   │   └── src/
│   │       ├── lib.rs
│   │       ├── keyboard.rs
│   │       ├── mouse.rs
│   │       ├── touch.rs               # 触摸输入 (Android)
│   │       ├── gamepad.rs
│   │       └── bindings.rs            # 输入映射层
│   │
│   ├── ui/                             # 运行时 UI
│   │   ├── Cargo.toml
│   │   └── src/
│   │       ├── lib.rs
│   │       ├── canvas.rs              # UI 画布
│   │       ├── widgets/               # 内置控件
│   │       └── layout.rs              # 布局系统
│   │
│   ├── scripting/                      # 脚本模块
│   │   ├── Cargo.toml                  # deps: wasmtime
│   │   └── src/
│   │       ├── lib.rs
│   │       ├── runtime.rs             # wasmtime 运行时
│   │       ├── engine_api.rs          # 引擎 API 绑定到 WASM
│   │       ├── module.rs              # WASM 模块管理
│   │       └── hot_reload.rs          # 脚本热重载
│   │
│   ├── animation/                      # 动画模块
│   │   ├── Cargo.toml
│   │   └── src/
│   │       ├── lib.rs
│   │       ├── skeleton.rs            # 骨骼
│   │       ├── skin.rs                # 蒙皮
│   │       ├── clip.rs                # 动画片段
│   │       ├── blend_tree.rs          # 混合树
│   │       └── playback.rs            # 播放控制
│   │
│   ├── particle/                       # 粒子系统
│   │   ├── Cargo.toml
│   │   └── src/
│   │       ├── lib.rs
│   │       ├── emitter.rs             # 发射器
│   │       ├── gpu_particles.rs       # GPU 粒子计算
│   │       └── modules.rs             # 粒子模块
│   │
│   ├── ai_behavior/                    # AI 行为系统
│   │   ├── Cargo.toml
│   │   └── src/
│   │       ├── lib.rs
│   │       ├── behavior_tree.rs       # 行为树
│   │       ├── fsm.rs                 # 有限状态机
│   │       ├── utility_ai.rs          # Utility AI
│   │       ├── perception.rs          # 感知系统 (视觉/听觉)
│   │       └── blackboard.rs          # 黑板 (共享数据)
│   │
│   ├── navigation/                     # 导航模块
│   │   ├── Cargo.toml
│   │   └── src/
│   │       ├── lib.rs
│   │       ├── navmesh.rs             # 导航网格
│   │       ├── pathfinding.rs         # A* 寻路
│   │       └── crowd.rs               # 群体避让
│   │
│   ├── network/                        # 网络模块
│   │   ├── Cargo.toml
│   │   └── src/
│   │       ├── lib.rs
│   │       ├── transport.rs           # TCP/UDP/WebSocket 传输
│   │       ├── replication.rs         # 状态同步
│   │       ├── rpc.rs                 # 远程过程调用
│   │       ├── room.rs                # 房间管理
│   │       └── net_config.rs          # 网络配置
│   │
│   └── vfx/                            # 视觉效果模块
│       ├── Cargo.toml
│       └── src/
│           ├── lib.rs
│           ├── post_process.rs        # 后处理效果栈
│           ├── shader_graph.rs        # 着色器图
│           └── fx_manager.rs          # 效果管理器
│
├── editor/                             # 编辑器 (桌面 + Android)
│   ├── Cargo.toml
│   └── src/
│       ├── lib.rs                     # Editor trait 实现
│       ├── scene_view/                # 场景视图
│       │   ├── mod.rs
│       │   ├── viewport.rs            # 视口渲染
│       │   ├── gizmo.rs               # 变换操控器
│       │   └── selection.rs           # 选择系统
│       ├── hierarchy/                 # 实体树形面板
│       │   └── mod.rs
│       ├── inspector/                 # 属性检查器
│       │   └── mod.rs
│       ├── asset_browser/             # 资源浏览器
│       │   └── mod.rs
│       ├── graph_editor/              # 图编辑器 (行为树/材质)
│       │   └── mod.rs
│       ├── code_editor/               # 代码编辑器
│       │   └── mod.rs
│       ├── console/                   # 控制台面板
│       │   └── mod.rs
│       ├── overlay/                   # Android 覆盖层编辑器
│       │   └── mod.rs
│       └── workspace/                 # 编辑器工作区管理
│           └── mod.rs
│
├── ai_agent/                           # AI Agent 模块
│   ├── Cargo.toml
│   └── src/
│       ├── lib.rs                     # Module trait 实现
│       ├── llm/                       # LLM 运行时
│       │   ├── mod.rs
│       │   ├── local.rs              # ollama 本地推理
│       │   ├── remote.rs             # OpenAI/Claude API
│       │   └── prompt.rs             # 系统提示管理
│       ├── tools/                     # 工具系统
│       │   ├── mod.rs
│       │   ├── scene_tool.rs         # 场景操作工具
│       │   ├── code_tool.rs          # 代码读写工具
│       │   ├── debug_tool.rs         # 调试工具
│       │   ├── asset_tool.rs         # 资源管理工具
│       │   ├── query_tool.rs         # 引擎状态查询
│       │   └── behavior_tool.rs      # 行为树生成工具
│       ├── context/                   # 上下文管理
│       │   ├── mod.rs
│       │   ├── scene_context.rs      # 场景上下文
│       │   └── project_context.rs    # 项目上下文
│       ├── chat/                      # 对话接口
│       │   └── mod.rs
│       └── sandbox/                   # 代码沙箱
│           └── mod.rs
│
├── assets/                             # 引擎自带资源
│   ├── default/                       # 默认资源 (材质/模型/纹理)
│   └── shaders/                       # 内置着色器
│
├── examples/                           # 示例项目
│   ├── sprite2d/
│   ├── minimal_3d/
│   ├── platformer/
│   └── ai_agent_demo/
│
└── docs/
    └── architecture.md
```

---

## 4. 内核 (Kernel)

### 4.1 Kernel 主结构

```rust
// kernel/src/lib.rs

pub struct Kernel {
    /// ECS World — 所有实体、组件、资源的容器
    pub world: World,

    /// 模块注册表
    modules: ModuleRegistry,

    /// 事件总线
    event_bus: EventBus,

    /// 主调度 (ECS Systems)
    schedule: Schedule,

    /// 任务系统
    task_system: TaskSystem,

    /// 资源管理器
    resource_manager: ResourceManager,

    /// 平台抽象
    platform: Box<dyn Platform>,

    /// 性能分析器
    profiler: FrameProfiler,

    /// 控制台/日志
    console: Console,

    /// 是否正在运行
    running: bool,
}
```

### 4.2 Kernel 生命周期

```rust
impl Kernel {
    /// 创建引擎内核
    pub fn new(platform: Box<dyn Platform>) -> Self {
        let mut kernel = Self {
            world: World::new(),
            modules: ModuleRegistry::new(),
            event_bus: EventBus::new(),
            schedule: Schedule::default(),
            task_system: TaskSystem::new(num_cpus::get()),
            resource_manager: ResourceManager::new(),
            platform,
            profiler: FrameProfiler::new(),
            console: Console::new(),
            running: false,
        };

        // 注册内核级资源
        kernel.world.insert_resource(Time::default());
        kernel.world.insert_resource(FrameStats::default());

        // 通过 linkme 自动发现并注册所有静态模块
        for module_fn in MODULES {
            let module = module_fn(&mut kernel);
            kernel.modules.register(module);
        }

        kernel
    }

    /// 主循环 — 驱动整个引擎
    pub fn run(&mut self) -> Result<()> {
        self.running = true;

        // 1. 加载所有模块 (按依赖拓扑排序)
        self.modules.load_all(self)?;
        self.event_bus.publish(EngineEvent::ModulesLoaded);

        // 2. 主循环
        while self.running {
            self.profiler.begin_frame();

            let dt = self.platform.time().delta_seconds();

            // 帧开始事件
            self.event_bus.publish(FrameEvent::Start(dt));

            // 更新所有模块
            for module in self.modules.iter_mut() {
                module.update(self, dt)?;
            }

            // 运行 ECS Schedule (并行)
            self.task_system.run_schedule(&mut self.world, &mut self.schedule);

            // 刷新异步事件队列
            self.event_bus.flush();

            // 帧结束事件
            self.event_bus.publish(FrameEvent::End(dt));

            self.profiler.end_frame();
            self.platform.window_system().poll_events();
        }

        // 3. 卸载模块 (逆序)
        self.modules.unload_all(self)?;

        Ok(())
    }
}
```

### 4.3 模块管理器

```rust
use linkme::distributed_slice;

/// 模块 trait — 所有模块必须实现
pub trait Module: Send + 'static {
    fn name(&self) -> &'static str;
    fn dependencies(&self) -> &[&'static str] { &[] }
    fn priority(&self) -> u32 { 0 }

    fn init(&mut self, kernel: &mut Kernel) -> Result<()>;
    fn update(&mut self, kernel: &mut Kernel, dt: f32) -> Result<()>;
    fn shutdown(&mut self, kernel: &mut Kernel) -> Result<()>;
}

/// linkme 分布式切片: 编译期自动注册模块工厂
#[distributed_slice]
pub static MODULES: [fn(&mut Kernel) -> Box<dyn Module>] = [..];

/// 运行时模块注册表
pub struct ModuleRegistry {
    entries: Vec<ModuleEntry>,
    by_name: HashMap<&'static str, usize>,
}

struct ModuleEntry {
    module: Box<dyn Module>,
    state: ModuleState,
}

impl ModuleRegistry {
    pub fn register(&mut self, module: Box<dyn Module>) {
        let idx = self.entries.len();
        self.entries.push(ModuleEntry {
            module,
            state: ModuleState::Registered,
        });
        self.by_name.insert(self.entries[idx].module.name(), idx);
    }

    pub fn load_all(&mut self, kernel: &mut Kernel) -> Result<()> {
        // 拓扑排序 (依赖解析 + 优先级排序)
        let order = self.topological_sort()?;

        for &idx in &order {
            self.entries[idx].module.init(kernel)?;
            self.entries[idx].state = ModuleState::Active;
        }
        Ok(())
    }

    pub fn unload_all(&mut self, kernel: &mut Kernel) -> Result<()> {
        // 逆序卸载
        for entry in self.entries.iter_mut().rev() {
            entry.module.shutdown(kernel)?;
            entry.state = ModuleState::Unloaded;
        }
        Ok(())
    }

    pub fn get<T: Module>(&self) -> Option<&T> { ... }
    pub fn get_mut<T: Module>(&mut self) -> Option<&mut T> { ... }
}
```

### 4.4 事件总线

```rust
/// 混合 IPC 模型:
/// - 核心↔核心: 直接函数调用
/// - 核心↔模块: Module trait 方法
/// - 模块↔模块: EventBus (发布/订阅)

pub struct EventBus {
    channels: HashMap<TypeId, Box<dyn AnyChannel>>,
    pending: Vec<Box<dyn AnyEvent>>,  // 异步队列
}

pub trait Event: Send + 'static {}
pub trait EventHandler<E: Event>: Send + 'static {
    fn handle(&mut self, event: &E);
}

impl EventBus {
    /// 注册监听器
    pub fn listen<E: Event, H: EventHandler<E>>(&mut self, handler: H) { ... }

    /// 同步发布 — 立即分派
    pub fn publish<E: Event>(&mut self, event: E) { ... }

    /// 异步发送 — 下一帧 flush 时处理
    pub fn send<E: Event>(&mut self, event: E) { ... }

    /// 处理异步事件
    pub fn flush(&mut self) { ... }
}

// ── 内核预定义事件 ──

pub enum EngineEvent {
    ModulesLoaded,
    ModuleLoaded { name: &'static str },
    ModuleUnloaded { name: &'static str },
    Shutdown,
}

pub enum FrameEvent {
    Start(f32),  // dt
    End(f32),    // dt
}

pub enum InputEvent {
    KeyDown { key: KeyCode },
    KeyUp { key: KeyCode },
    MouseMove { x: f32, y: f32 },
    MouseDown { button: MouseButton },
    MouseUp { button: MouseButton },
    Touch { phase: TouchPhase, x: f32, y: f32, id: u64 },
    Gamepad { button: GamepadButton, value: f32 },
}

pub enum PhysicsEvent {
    ContactStarted(Entity, Entity),
    ContactEnded(Entity, Entity),
}

pub enum NetworkEvent {
    Connected(u64),   // player id
    Disconnected(u64),
    Message(u64, Vec<u8>),
}
```

### 4.5 任务系统

```rust
/// 基于 bevy_ecs ComputeTaskPool 的并行任务调度
pub struct TaskSystem {
    pool: ComputeTaskPool,
}

impl TaskSystem {
    pub fn new(thread_count: usize) -> Self {
        Self {
            pool: ComputeTaskPool::new(thread_count),
        }
    }

    /// 并行执行 ECS Schedule
    pub fn run_schedule(&self, world: &mut World, schedule: &mut Schedule) {
        schedule.run(world);
    }

    /// 提交并行任务
    pub fn spawn<T: Task + 'static>(&self, task: T) { ... }
}

/// 调度模式控制
pub enum ScheduleMode {
    FixedUpdate(f32),   // 固定时间步
    Variable,           // 跟随实际帧时间
}
```

### 4.6 资源管理器

```rust
pub struct ResourceManager {
    cache: HashMap<AssetId, AssetEntry>,
    loaders: HashMap<&'static str, Box<dyn AssetLoader>>,
    watchers: Vec<Box<dyn FileWatcher>>,
}

pub trait AssetLoader: Send + 'static {
    fn supported_extensions(&self) -> &[&'static str];
    fn load(&self, path: &Path, ctx: &LoadContext) -> Result<Box<dyn Asset>>;
}

pub struct AssetEntry {
    asset: Box<dyn Asset>,
    ref_count: usize,
    last_accessed: Instant,
    hot_reload: bool,
}

impl ResourceManager {
    pub fn load<T: Asset>(&mut self, path: &Path) -> Result<Handle<T>>;
    pub fn unload(&mut self, handle: &HandleId);
    pub fn reload_changed(&mut self);  // 热重载
}
```

### 4.7 性能分析器

```rust
pub struct FrameProfiler {
    frame_time: Histogram,
    module_times: HashMap<&'static str, Duration>,
    gpu_times: Vec<wgpu::Timestamp>,
}

impl FrameProfiler {
    pub fn begin_frame(&mut self);
    pub fn end_frame(&mut self);
    pub fn begin_module(&mut self, name: &'static str);
    pub fn end_module(&mut self, name: &'static str);
    pub fn report(&self) -> FrameReport;
}
```

---

## 5. 模块系统

### 5.1 模块注册 (linkme)

```rust
// 在任何模块 crate 中:

use gearx_kernel::{Kernel, MODULES};
use linkme::distributed_slice;

#[distributed_slice(MODULES)]
fn register_render_module(kernel: &mut Kernel) -> Box<dyn Module> {
    Box::new(RenderModule::new(kernel))
}
```

编译器将所有 `#[distributed_slice(MODULES)]` 元素自动收集为静态数组,
`Kernel::new()` 时遍历注册。**零运行时开销**, 无初始化顺序依赖问题。

### 5.2 Feature Gate 控制

```toml
# kernel/Cargo.toml
[features]
default = [
    "render", "physics", "audio", "input", "ui",
    "animation", "particle", "vfx",
]
render = ["dep:gearx_render"]
physics = ["dep:gearx_physics"]
audio = ["dep:gearx_audio"]
input = ["dep:gearx_input"]
ui = ["dep:gearx_ui"]
animation = ["dep:gearx_animation"]
particle = ["dep:gearx_particle"]
ai_behavior = ["dep:gearx_ai_behavior"]
navigation = ["dep:gearx_navigation"]
network = ["dep:gearx_network"]
scripting = ["dep:gearx_scripting"]
vfx = ["dep:gearx_vfx"]

# 编辑器 + AI Agent 可选
editor = ["dep:gearx_editor"]
ai_agent = ["dep:gearx_ai_agent"]
```

```bash
# 构建精简版本 (比如只做 2D 游戏)
cargo build --no-default-features --features "render,input,audio,physics"
# 或完整版本
cargo build --features "editor,ai_agent,network"
```

### 5.3 模块间依赖示意

```
render ──┬── input     (窗口事件 → 渲染)
         ├── animation (骨骼 → 渲染)
         ├── particle  (粒子 → 渲染)
         └── ui        (UI → 渲染)

physics ─── render  (物理调试绘制)

audio ─── (独立)

ai_behavior ─┬─ navigation (行为 → 寻路)
             ├─ physics    (行为 → 物理查询)
             └─ perception (感知输入)

network ─── scripting (RPC 绑定)

scripting ─── render / physics / audio / input (引擎 API 暴露到 WASM)
```

---

## 6. AI Agent 系统

### 6.1 系统架构

AI Agent 是内核级模块, 通过 feature gate `ai_agent` 控制, 桌面和 Android 上均可使用。

```
┌─────────────────────────────────────────────────────────┐
│                    AI Agent System                       │
│                                                         │
│  ┌──────────────────────────────────────────────────┐  │
│  │                  LLM Runtime                      │  │
│  │  ┌──────────────┐  ┌──────────────────────────┐  │  │
│  │  │  Local (ollama)│  │  Remote (OpenAI/Claude)  │  │  │
│  │  │  本地推理      │  │  API 调用                │  │  │
│  │  └──────────────┘  └──────────────────────────┘  │  │
│  │       可切换 (Kernel::set_llm_provider())         │  │
│  └──────────────────────────────────────────────────┘  │
│                                                         │
│  ┌──────────────────────────────────────────────────┐  │
│  │                  Tool System                      │  │
│  │                                                   │  │
│  │  ┌──────────┐ ┌──────────┐ ┌──────────┐         │  │
│  │  │SceneTool │ │CodeTool  │ │DebugTool │         │  │
│  │  │创建/修改   │ │读写代码   │ │分析日志   │         │  │
│  │  │实体/组件   │ │生成系统   │ │插入断点   │         │  │
│  │  ├──────────┤ ├──────────┤ ├──────────┤         │  │
│  │  │AssetTool │ │QueryTool │ │Behavior  │         │  │
│  │  │资源管理   │ │引擎查询   │ │行为树生成 │         │  │
│  │  └──────────┘ └──────────┘ └──────────┘         │  │
│  └──────────────────────────────────────────────────┘  │
│                                                         │
│  ┌──────────────────────────────────────────────────┐  │
│  │              Context Manager                      │  │
│  │  • 场景上下文 (当前实体树 + 组件数据)               │  │
│  │  • 项目上下文 (文件结构 + 代码库)                   │  │
│  │  • 引擎上下文 (API 文档 + 错误日志)                 │  │
│  └──────────────────────────────────────────────────┘  │
│                                                         │
│  ┌──────────────────────────────────────────────────┐  │
│  │              Chat Interface                       │  │
│  │  (编辑器面板 / Android 覆盖层 / CLI)                │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

### 6.2 Agent Module Trait 实现

```rust
// ai_agent/src/lib.rs

pub struct AiAgentModule {
    llm: Box<dyn LlmProvider>,
    tools: ToolRegistry,
    context: ContextManager,
    chat_history: Vec<ChatMessage>,
}

#[async_trait]
pub trait LlmProvider: Send + 'static {
    async fn chat(&self, messages: &[ChatMessage], tools: &[ToolDef])
        -> Result<LlmResponse>;
    fn name(&self) -> &'static str;
}

pub struct OllamaProvider {
    client: ollama_rs::Ollama,
    model: String,
}

pub struct RemoteApiProvider {
    api_key: String,
    endpoint: String,
    model: String,
}

impl Module for AiAgentModule {
    fn name(&self) -> &'static str { "ai_agent" }

    fn init(&mut self, kernel: &mut Kernel) -> Result<()> {
        // 注册所有工具
        self.tools.register(SceneTool::new(kernel));
        self.tools.register(CodeTool::new());
        self.tools.register(DebugTool::new(kernel));
        self.tools.register(AssetTool::new(kernel));
        self.tools.register(QueryTool::new(kernel));
        self.tools.register(BehaviorTool::new(kernel));

        // 注册为 ECS Resource (其他模块可调用)
        kernel.world.insert_resource(AiAgentHandle {
            send: self.channel.0.clone(),
        });

        Ok(())
    }

    fn update(&mut self, kernel: &mut Kernel, _dt: f32) -> Result<()> {
        // 处理待处理的 AI 请求
        while let Some(request) = self.pending_requests.pop_front() {
            let response = self.process_request(request);
            self.event_bus.publish(AiResponse(response));
        }
        Ok(())
    }
}
```

### 6.3 工具定义

```rust
// ai_agent/src/tools/mod.rs

pub struct ToolDef {
    pub name: &'static str,
    pub description: &'static str,
    pub parameters: Vec<ParamDef>,
}

pub trait AgentTool: Send + Sync {
    fn def(&self) -> ToolDef;
    fn execute(&self, params: &HashMap<String, String>) -> Result<ToolResult>;
}

// ── SceneTool ──

pub struct SceneTool {
    world: *mut World,  // 安全借用
}

impl AgentTool for SceneTool {
    fn def(&self) -> ToolDef {
        ToolDef {
            name: "scene_create_entity",
            description: "在场景中创建一个新实体，可附加组件",
            parameters: vec![
                ParamDef::string("name", "实体名称"),
                ParamDef::optional("parent", "父实体 ID"),
            ],
        }
    }

    fn execute(&self, params: &HashMap<String, String>) -> Result<ToolResult> {
        let world = unsafe { &mut *self.world };
        let entity = world.spawn((
            Name::new(params.get("name").unwrap()),
            Transform::default(),
        ));
        Ok(ToolResult::entity(entity.id()))
    }
}

// ── CodeTool ──

pub struct CodeTool;

impl AgentTool for CodeTool {
    fn def(&self) -> ToolDef {
        ToolDef {
            name: "code_read_file",
            description: "读取游戏项目中的代码文件",
            parameters: vec![
                ParamDef::string("path", "相对于项目根目录的文件路径"),
            ],
        }
    }

    fn execute(&self, params: &HashMap<String, String>) -> Result<ToolResult> {
        let path = params.get("path").unwrap();
        let content = std::fs::read_to_string(path)?;
        Ok(ToolResult::text(content))
    }
}

// ── BehaviorTool (行为树生成) ──

pub struct BehaviorTool;

impl AgentTool for BehaviorTool {
    fn def(&self) -> ToolDef {
        ToolDef {
            name: "behavior_generate_tree",
            description: "根据自然语言描述生成行为树",
            parameters: vec![
                ParamDef::string("description", "行为描述, 如'巡逻并攻击视野内的敌人'"),
            ],
        }
    }

    fn execute(&self, params: &HashMap<String, String>) -> Result<ToolResult> {
        let desc = params.get("description").unwrap();
        let bt = BehaviorTreeGenerator::from_description(desc);
        Ok(ToolResult::behavior_tree(bt))
    }
}
```

### 6.4 Agent 工作流示例

```
用户: "在 (0, 0, 0) 创建一个玩家角色，加一个摄像机跟着它"

AI Agent:
  ├─ 调用 QueryTool: 查询当前场景状态
  ├─ 调用 SceneTool: spawn_entity("Player", Transform::new(0,0,0))
  ├─ 调用 SceneTool: add_component(Player, Camera)
  ├─ 调用 CodeTool: 生成 player_controller.wasm 脚本框架
  └─ 回复用户: "已创建玩家角色和跟随摄像机"
```

---

## 7. 编辑器系统

### 7.1 编辑器架构

编辑器是可选系统, 通过 feature gate `editor` 控制。同一二进制文件通过模式切换:

```rust
pub enum EngineMode {
    /// 编辑模式 — 编辑器 UI 可见
    Edit,
    /// 运行模式 — 隐藏编辑器, 运行游戏
    Play,
    /// 暂停 (运行中暂停)
    Pause,
}
```

### 7.2 桌面编辑器

桌面版编辑器是完整的可视化开发环境:

```
┌─────────────────────────────────────────────────────────┐
│  Menu Bar  [File] [Edit] [View] [Tools] [AI] [Help]    │
├──────────┬──────────────────────────────┬───────────────┤
│          │                              │               │
│ Hierarchy│       Scene View             │  Inspector    │
│          │       (3D/2D视口)            │               │
│  - 场景   │                              │   Transform   │
│  - 玩家   │    ┌─────────────────┐      │   Position    │
│  - 灯光   │    │  Game Viewport   │      │   Rotation    │
│  - 地面   │    │                 │      │   Scale       │
│          │    └─────────────────┘      │               │
│          │                              │   Mesh        │
│          │                              │   ─────────   │
│          │                              │   model.gltf  │
│          │                              │               │
├──────────┴──────────────────────────────┴───────────────┤
│  Console  │  Asset Browser  │  AI Agent Chat             │
└─────────────────────────────────────────────────────────┘
```

### 7.3 Android 覆盖层编辑器

Android 上用半透明覆盖层展示编辑器 UI:

```
┌─────────────────────────────────────────┐
│  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  │
│  ▓▓  游戏画面 (半透明覆盖层之下)    ▓▓  │
│  ▓▓                               ▓▓  │
│  ┌─────────────────────────────┐  ▓▓  │
│  │  [✕] AI Agent Chat          │  ▓▓  │
│  │  ─────────────────────────  │  ▓▓  │
│  │  > 帮我创建一个旋转平台      │  ▓▓  │
│  │  ✓ 已创建, 添加了Rotate...  │  ▓▓  │
│  │  [┃输入消息...] [发送]      │  ▓▓  │
│  └─────────────────────────────┘  ▓▓  │
│  ▓▓                     ▓▓           │
│  [≡ 菜单]  [▶ 运行]  [AI]  [✎ 代码]   │
└─────────────────────────────────────────┘
```

### 7.4 Edit/Play 模式切换

```rust
impl Kernel {
    pub fn set_mode(&mut self, mode: EngineMode) {
        match mode {
            EngineMode::Edit => {
                // 暂停物理/脚本更新
                // 显示编辑器 UI
                // 启用场景编辑
            }
            EngineMode::Play => {
                // 隐藏编辑器 UI
                // 开始物理/脚本/网络
                // 锁定场景编辑
            }
            EngineMode::Pause => {
                // 暂停所有更新
                // 保持渲染
            }
        }
        self.mode = mode;
        self.event_bus.publish(EngineEvent::ModeChanged(mode));
    }
}
```

---

## 8. 运行时模块

### 8.1 Render 模块

```rust
// modules/render/src/lib.rs

pub struct RenderModule {
    instance: wgpu::Instance,
    surface: wgpu::Surface<'static>,
    device: wgpu::Device,
    queue: wgpu::Queue,
    render_graph: RenderGraph,
}

impl Module for RenderModule {
    fn name(&self) -> &'static str { "render" }
    fn dependencies(&self) -> &[&'static str] { &["input"] }

    fn init(&mut self, kernel: &mut Kernel) -> Result<()> {
        // 注册 ECS 组件
        kernel.world.register_component::<Mesh>();
        kernel.world.register_component::<Material>();
        kernel.world.register_component::<Transform>();
        kernel.world.register_component::<Camera>();
        kernel.world.register_component::<Light>();

        // 注册渲染 System 到 Schedule
        kernel.schedule.add_systems((
            render_prepare_system,
            render_execute_system.after(render_prepare_system),
        ));

        Ok(())
    }
}

/// 渲染管线流水线
///
/// Frame Prepare Phase:
///   1. Culling (视锥剔除 + 遮挡剔除)
///   2. 排序 (透明/不透明)
///   3. 更新 GPU 缓冲区
///
/// Frame Execute Phase:
///   4. Shadow Pass → ShadowMap
///   5. Geometry Pass → GBuffer
///   6. Lighting Pass → PBR Shading
///   7. Transparent Pass
///   8. Post-Processing → Bloom / Tonemap / FXAA
///   9. UI Overlay
///  10. Present
```

### 8.2 Physics 模块

```rust
// modules/physics/src/lib.rs

pub struct PhysicsModule {
    pipeline: rapier3d::pipeline::PhysicsPipeline,
    gravity: rapier3d::dynamics::Gravity,
    integration: rapier3d::dynamics::IntegrationParameters,
    broad_phase: rapier3d::pipeline::BroadPhase,
    narrow_phase: rapier3d::pipeline::NarrowPhase,
    island_manager: rapier3d::dynamics::IslandManager,
    bodies: rapier3d::dynamics::RigidBodySet,
    colliders: rapier3d::geometry::ColliderSet,
    joints: rapier3d::dynamics::ImpulseJointSet,
    query: rapier3d::pipeline::QueryPipeline,
}

impl Module for PhysicsModule {
    fn name(&self) -> &'static str { "physics" }
    fn dependencies(&self) -> &[&'static str] { &["render"] }

    fn init(&mut self, kernel: &mut Kernel) -> Result<()> {
        kernel.world.register_component::<RigidBody>();
        kernel.world.register_component::<Collider>();
        self.gravity = rapier3d::dynamics::Gravity::new(
            &na::Vector3::new(0.0, -9.81, 0.0)
        );
        Ok(())
    }

    fn update(&mut self, kernel: &mut Kernel, dt: f32) -> Result<()> {
        // ECS → Rapier 同步 (Transform → Body)
        sync_ecs_to_physics(&mut kernel.world, &mut self.bodies);

        // 物理步进
        self.pipeline.step(
            &self.gravity,
            &self.integration,
            &mut self.island_manager,
            &mut self.broad_phase,
            &mut self.narrow_phase,
            &mut self.bodies,
            &mut self.colliders,
            &mut self.joints,
            &mut rapier3d::dynamics::CCDSolver::new(),
            None,
            &(),
            &(),
        );

        // Rapier → ECS 同步 (Body → Transform)
        sync_physics_to_ecs(&mut kernel.world, &self.bodies);

        // 发布碰撞事件
        kernel.event_bus.publish(PhysicsEvent::ContactStarted(a, b));

        Ok(())
    }
}
```

### 8.3 Scripting 模块 (WASM)

```rust
// modules/scripting/src/lib.rs

use wasmtime::{Engine, Module, Instance, Store, Linker};

pub struct ScriptingModule {
    engine: Engine,
    wasm_modules: HashMap<String, WasmScript>,
}

struct WasmScript {
    instance: Instance,
    store: Store<ScriptContext>,
    update_fn: TypedFunc<(f32,), ()>,
}

impl Module for ScriptingModule {
    fn name(&self) -> &'static str { "scripting" }

    fn init(&mut self, kernel: &mut Kernel) -> Result<()> {
        kernel.world.insert_resource(ScriptEngineHandle { ... });
        Ok(())
    }

    fn update(&mut self, kernel: &mut Kernel, dt: f32) -> Result<()> {
        for (_, script) in &mut self.wasm_modules {
            script.update_fn.call(&mut script.store, (dt,))?;
        }
        Ok(())
    }
}

/// WASM 模块可以访问的引擎 API
///
/// 通过 wasmtime::Linker 将以下函数注入 WASM 实例:
///
/// ```rust
/// // WASM 导入的引擎 API
/// extern "C" {
///     fn spawn_entity(name_ptr: i32, name_len: i32) -> i32;     // → EntityID
///     fn get_component(entity: i32, comp_id: i32, out: i32);     // → ComponentData
///     fn set_component(entity: i32, comp_id: i32, data: i32);    // ← ComponentData
///     fn send_event(event_ptr: i32, event_len: i32);
///     fn log_message(level: i32, msg_ptr: i32, msg_len: i32);
///     fn get_delta_time() -> f32;
///     fn input_is_key_down(key: i32) -> i32;
/// }
/// ```
```

### 8.4 AI Behavior 模块

```rust
// modules/ai_behavior/src/lib.rs

pub struct AiBehaviorModule;

impl Module for AiBehaviorModule {
    fn name(&self) -> &'static str { "ai_behavior" }

    fn init(&mut self, kernel: &mut Kernel) -> Result<()> {
        kernel.world.register_component::<BehaviorTreeAgent>();
        kernel.world.register_component::<FSMAgent>();

        // 注册行为树 System
        kernel.schedule.add_systems((
            update_behavior_trees,
            update_fsms,
            perception_system,
        ));

        Ok(())
    }
}

// ── 行为树节点类型 ──

pub enum BehaviorNode {
    /// 序列: 依次执行子节点, 全部成功→成功
    Sequence(Vec<BehaviorNode>),
    /// 选择: 依次执行子节点, 一个成功→成功
    Selector(Vec<BehaviorNode>),
    /// 条件
    Condition(Box<dyn Condition>),
    /// 动作
    Action(Box<dyn Action>),
    /// 并行
    Parallel(Vec<BehaviorNode>),
    /// 反转
    Inverter(Box<BehaviorNode>),
    /// 直到成功
    UntilSuccess(Box<BehaviorNode>),
}

pub trait Condition: Send + Sync {
    fn evaluate(&self, world: &World, entity: Entity) -> bool;
}

pub trait Action: Send + Sync {
    fn execute(&mut self, world: &mut World, entity: Entity) -> BtStatus;
}

// ── FSM ──

pub struct FSMAgent {
    current_state: StateId,
    states: HashMap<StateId, FsmState>,
    transitions: Vec<FsmTransition>,
}

struct FsmState {
    id: StateId,
    on_enter: Option<Box<dyn Action>>,
    on_update: Box<dyn Action>,
    on_exit: Option<Box<dyn Action>>,
}

struct FsmTransition {
    from: StateId,
    to: StateId,
    condition: Box<dyn Condition>,
}
```

---

## 9. 数据层

### 9.1 场景系统

```rust
/// 场景格式 (序列化到 .scene 文件)
#[derive(Serialize, Deserialize)]
pub struct Scene {
    pub version: u32,
    pub entities: Vec<EntityDef>,
    pub resources: Vec<ResourceDef>,
    pub scripts: Vec<ScriptBinding>,
}

#[derive(Serialize, Deserialize)]
pub struct EntityDef {
    pub id: Option<u64>,
    pub name: String,
    pub parent: Option<u64>,
    pub components: Vec<ComponentDef>,
}

#[derive(Serialize, Deserialize)]
pub struct ComponentDef {
    pub type_name: String,
    pub data: serde_json::Value, // 动态类型数据
}

/// 预制体系统 — 可复用的实体模板
pub struct Prefab {
    pub scene: Scene,
    pub exports: Vec<String>,  // 暴露的可覆写属性
}
```

### 9.2 Asset Pipeline

```
源文件 (.gltf/.png/.wav 等)         导入器 (Importer)
        │                                   │
        ▼                                   ▼
  源资产缓存                          处理 (压缩/转换)
        │                                   │
        ▼                                   ▼
  引擎格式资产 (.mesh/.tex/.sound)    缓存到项目目录
        │                                   │
        ▼                                   ▼
  ResourceManager                     热重载 (文件监控)
        │
        ▼
  游戏运行时
```

```rust
pub struct AssetPipeline {
    importers: HashMap<&'static str, Box<dyn Importer>>,
    processors: Vec<Box<dyn Processor>>,
    file_watcher: FileWatcher,
}

pub trait Importer: Send + 'static {
    fn supported_extensions(&self) -> &[&'static str];
    fn import(&self, source: &Path, dest: &Path) -> Result<()>;
}

// 内置导入器
pub struct GltfImporter;    // .gltf/.glb → .mesh + .material + .texture
pub struct ImageImporter;   // .png/.jpg/.webp → .tex
pub struct AudioImporter;   // .wav/.ogg/.mp3 → .sound
pub struct FontImporter;    // .ttf/.otf → .font
```

---

## 10. 构建与工作流

### 10.1 桌面开发流程

```bash
# 快速迭代
cargo run -p example_platformer

# 带编辑器和 AI Agent
cargo run --features "editor,ai_agent" -p example_platformer

# 发布模式
cargo build --release --features "editor,ai_agent,network"

# 构建 WASM 脚本 (给 scripting 模块用)
cd game_scripts && wasm-pack build --target web
```

### 10.2 Android 工作流

```bash
# 构建引擎 APK (Android 编辑器模式)
cargo apk run --features "editor,ai_agent"

# 构建发布 APK
cargo apk build --release

# 引擎 APK 结构
# gearx.apk/
# ├── lib/arm64-v8a/libgearx.so     # 引擎原生库
# ├── assets/
# │   ├── engine/default/           # 内置资源
# │   └── project/                  # 用户项目文件
# └── ...
```

### 10.3 Web 发布

```bash
# 构建 WASM 运行时
cargo build --target wasm32-unknown-unknown --release

# WASM 大小优化
wasm-opt -Oz target/wasm32-unknown-unknown/release/gearx.wasm -o gearx_opt.wasm
```

### 10.4 热重载工作流

```
文件修改 (代码/资源/脚本)
      │
      ▼
  FileWatcher 检测变更
      │
      ├── WASM 脚本 → 重新编译 → 加载新 WASM
      ├── 资源文件 → 刷新 ResourceManager 缓存
      ├── 场景文件 → 合并场景变更 (Editor 模式)
      └── 着色器 → 重新编译 wgpu ShaderModule
```

---

## 11. 架构决策记录 (ADR)

| ID | 决策 | 理由 | 影响 |
|----|------|------|------|
| ADR-001 | Rust 语言 | 内存安全 + 零开销 + 跨平台 | 学习曲线, 借用检查器 |
| ADR-002 | bevy_ecs 而非自研 ECS | 成熟稳定, 避免重复造轮子 | 依赖第三方, 需紧跟更新 |
| ADR-003 | wgpu 渲染后端 | 跨平台统一 API, Vulkan/Metal/DX12/WebGPU | 不如原生 Vulkan 灵活 |
| ADR-004 | 全静态编译 | 简化架构, 支持 WASM, 无 ABI 问题 | 二进制体积大, 无热插拔模块 |
| ADR-005 | WASM (wasmtime) 作脚本 | 跨平台, 多语言, 沙箱安全 | WASM 运行时开销 |
| ADR-006 | AI Agent 为内核模块 | 运行时也可访问, 深度集成 | LLM 调用是阻塞操作 |
| ADR-007 | 编辑器 + 运行时一体 | Edit/Play 切换无开销 | 二进制体积大 |
| ADR-008 | linkme 编译期模块注册 | 零开销, 无初始化顺序问题 | 链接器依赖 (Linux/macOS, Windows MSVC) |
| ADR-009 | 事件总线异步发布 | 降低模块间耦合 | 延迟一帧处理 |
| ADR-010 | Rapier 物理引擎 | 纯 Rust, 2D/3D 通用 | 不如 PhysX 生态丰富 |

---

*本文档对应 GearX v3 完整架构, 覆盖 19 个子系统、4 个目标平台、7 层架构。*
