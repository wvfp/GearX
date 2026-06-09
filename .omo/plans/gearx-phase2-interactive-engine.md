# GearX Phase 2 — Interactive Engine

## TL;DR

> **Quick Summary**: Transform the bootable skeleton into an interactive engine by integrating ECS, adding input handling, shader pipeline, basic asset loading, and window resize. End state: a window where you can move a textured sprite with keyboard/mouse.

> **Deliverables**:
> - ECS World + Schedule wired into Kernel (bevy_ecs systems running each frame)
> - Input module (keyboard + mouse events via winit → EventBus → ECS resources)
> - Window resize handling (swapchain recreation)
> - Shader compilation pipeline (load WGSL, create RenderPipeline)
> - Basic asset system (texture + mesh loading from file or embedded)
> - Triangle → textured quad → simple 3D mesh rendering
> - TaskSystem implementation (parallel job dispatch)
> - Phase 1 quality fixes (unwrap, transmute, missing #[must_use])

> **Estimated Effort**: Large (12-14 implementation tasks + 4 verification)
> **Parallel Execution**: YES — 4 waves, max 4 concurrent tasks
> **Critical Path**: quality fixes → ECS Kernel → Input Module → Shader Pipeline → Asset System → verification

---

## Context

### Phase 1 Recap

Phase 1 delivered a **bootable engine skeleton**:
- Cargo workspace with 3 crates (kernel, render, binary)
- Module system (linkme registry, Module trait, lifecycle)
- EventBus (sync publish/subscribe, TypeId-HashMap)
- Platform abstraction (winit desktop + Android/Web stubs)
- Kernel lifecycle (new/run/shutdown/set_mode)
- Render module (wgpu Instance→Adapter→Device→Surface, clear color)
- Desktop binary (winit window 1280×720 + event polling)
- Tracing logging + frame counter
- Unit tests (EventBus, ModuleRegistry) + integration test
- CI scripts (verify-phase1.sh + GitHub Actions)
- **21/21 tests pass, 0 build errors**

### Phase 1 Deferred (Phase 2 Scope)

| Item | Phase 1 Status | Why Deferred |
|------|---------------|-------------|
| bevy_ecs World + Schedule | Kernel has no ECS fields | Need stable kernel first |
| Input processing | Only CloseRequested handled | Platform events not wired |
| Window resize | Hardcoded 1280×720 | wgpu swapchain needs recreation |
| Shader compilation | No pipeline creation | Module was stub |
| Asset loading | Empty placeholder | No loader infrastructure |
| TaskSystem | Empty placeholder | Not needed for boot |
| Topological sort | Simple Vec order | Module count was 1 |
| Async EventBus | Sync publish only | Not needed for single-thread |
| Profiler | Basic frame counter | Defer to when performance matters |
| EngineMode logic | Stub (store + publish) | No editor to switch to yet |

### Phase 1 Quality Fixes (from F2 Review)

Before adding new features, fix these from F2:
- `desktop.rs:70` — `unwrap()` → proper error handling
- `desktop.rs:140` — `unsafe { transmute }` — safer lifetime management
- `registry.rs:107` — silent swallow → `tracing::warn!`
- `kernel.rs:132` — silent swallow → `tracing::warn!`
- Add `#[must_use]` to `len()`, `state()`, `mode()`, `is_running()`, `frame_count()`, `fps()`
- `render/src/lib.rs:148-153` — panicking linkme factory (design issue, may stay)

---

## Work Objectives

### Core Objective
Transform GearX from a "window with clear color" to an **interactive engine** where ECS drives gameplay, input affects state, and the renderer actually draws geometry. The engine should be able to load assets and display textured meshes on screen.

### Concrete Deliverables
1. ECS World + Schedule integrated into Kernel (bevy_ecs systems run each frame)
2. Input module handling keyboard/mouse → EventBus events → ECS resources
3. Window resize → wgpu swapchain recreation
4. Shader compilation pipeline (load WGSL → compile → create RenderPipeline)
5. Basic asset system (texture loader, mesh loader, embedded defaults)
6. Render triangle → textured quad → simple 3D mesh
7. TaskSystem basic implementation (parallel ECS schedule execution)
8. Phase 1 quality fixes
9. Integration tests for new subsystems

### Definition of Done
- [ ] `cargo run` shows a colored triangle/quad on screen (not just clear color)
- [ ] Keyboard arrow keys move the triangle/quad
- [ ] Window resize triggers swapchain recreation (no crash)
- [ ] Console shows FPS and input events
- [ ] ECS Schedule runs per-frame with user systems
- [ ] Assets load from disk (textures)
- [ ] `cargo test --workspace` — all tests pass
- [ ] `cargo build --workspace` — 0 errors, 0 warnings
- [ ] No `#![feature(...)]`

### Must Have
- ECS World + Schedule in Kernel, `run_schedule()` each frame
- `InputModule` that translates winit PlatformEvent → ECS resources
- `InputState` ECS resource (keyboard bitmask, mouse position)
- Window resize event → surface.configure() with new dimensions
- `RenderPipeline` creation from WGSL shader source
- Basic vertex + fragment shader (embedded WGSL)
- Triangle mesh rendering (vertex buffer + index buffer)
- Texture loading from PNG (image crate + wgpu texture)
- Textured quad rendering
- `AssetModule` with `TextureLoader` + `MeshLoader`
- TaskSystem with `ComputeTaskPool` and `run_schedule`
- Phase 1 quality fixes (unwrap → expect, transmute → safe pattern)

### Must NOT Have (Guardrails)
- NO physics, audio, network, AI Agent, scripting, animation, particles
- NO editor UI (stays in Phase 3)
- NO skeletal animation / skinning
- NO PBR / IBL lighting
- NO render graph (direct rendering for now)
- NO hot-reload / file watching
- NO threaded asset loading (blocking load on main thread is fine)
- NO WASM build (compile-check only)
- NO Android/Web runtime (compile-check only)
- NO `#![feature(...)]`

---

## Execution Strategy

### Parallel Execution Waves

```
Wave 0.5 (Phase 1 cleanup — 2 tasks, ALL PARALLEL):
├── Task Q1: Fix F2 critical issues (unwrap, transmute, #[must_use])
└── Task Q2: Fix F2 warnings (error swallows, clippy)

Wave 1 (ECS + Input — 3 tasks, 2 PARALLEL + 1 SEQUENTIAL):
├── Task 1: Wire bevy_ecs World + Schedule into Kernel
├── Task 2: InputModule (winit events → ECS resources) [parallel with T1]
└── Task 3: Window resize handling in RenderModule [parallel with T1, T2]

Wave 2 (Rendering — 3 tasks, ALL PARALLEL):
├── Task 4: Shader compilation pipeline
├── Task 5: Triangle + textured quad rendering
└── Task 6: Basic mesh rendering (vertex/index buffers)

Wave 3 (Assets + TaskSystem — 3 tasks, ALL PARALLEL):
├── Task 7: AssetModule with TextureLoader + MeshLoader
├── Task 8: TaskSystem with ComputeTaskPool
└── Task 9: ECS-driven demo (move triangle with keyboard)

Wave 4 (QA — 4 tasks, ALL PARALLEL):
├── Task 10: Unit tests for InputModule + AssetSystem
├── Task 11: Integration test: ECS → Input → Render
├── Task 12: Cross-compile CI update
└── Task 13: Build/run verification script

Wave FINAL (Verification — 4 parallel reviewers):
├── Task F1: Plan compliance audit (oracle)
├── Task F2: Code quality review (unspecified-high)
├── Task F3: Real manual QA (unspecified-high + bash)
└── Task F4: Scope fidelity check (deep)
-> Present results -> Get explicit user approval

Critical Path: Q1 → T1 → T5 → T9 → F1-F4
Parallel Speedup: ~60% faster than sequential
```

### Dependency Matrix

| Task | Depends On | Blocks |
|------|-----------|--------|
| Q1 | Phase 1 | Q2 |
| Q2 | Q1 | T1, T2 |
| T1 | Q2 | T3, T5, T9 |
| T2 | Q2 | T3, T9 |
| T3 | T1, T2 | T5 |
| T4 | T1 | T5 |
| T5 | T3, T4 | T9 |
| T6 | T4 | T7, T9 |
| T7 | T5, T6 | T9 |
| T8 | T1 | T9 |
| T9 | T5, T7, T8 | T10, F3 |
| T10 | T2, T7, T8 | T11 |
| T11 | T9 | F1-F4 |
| T12 | T9 | (none) |
| T13 | T9 | (none) |
| F1-F4 | All | User OK |

---

## Task Breakdown

### Wave 0.5 — Phase 1 Quality Fixes (ALL PARALLEL)

- [ ] Q1. **Fix F2 critical issues**
  - **What**: Fix the 2 critical items from F2 code quality review:
    1. `kernel/src/platform/desktop.rs:70` — Change `self.window.window_handle().unwrap()` to `self.window.window_handle().expect("winit window handle is always valid")`
    2. `kernel/src/platform/desktop.rs:140` — Replace `unsafe { std::mem::transmute(on_event) }` with a safer pattern: use `Box::into_raw(Box::new(on_event))` → `Box::from_raw` or restructure ownership to avoid the `'static` cast entirely
  - **Category**: `quick`

- [ ] Q2. **Fix F2 warnings + clippy**
  - **What**: Fix 9 warnings + 6 info items from F2:
    1. Add `tracing::warn!()` to error swallows in registry.rs:107, desktop.rs:142, kernel.rs:132
    2. Add `#[must_use]` to `len()`, `state()`, `mode()`, `is_running()`, `frame_count()`, `fps()`
    3. Implement `Default` for `DesktopTime` and `FrameCounter`
    4. Add `is_empty()` to `ModuleRegistry`
    5. Change `get()`/`get_mut()` return `&dyn Module` instead of `&Box<dyn Module>`
    6. Fix double blank line in desktop.rs
    7. Add `#[must_use]` to all pure getters
  - **Category**: `quick`

### Wave 1 — ECS + Input + Resize

- [ ] 1. **Wire bevy_ecs World + Schedule into Kernel**
  - **What**: Add `world: World` and `schedule: Schedule` fields to `Kernel`. Insert core resources (`Time`, `FrameStats`). Run `self.schedule.run(&mut self.world)` each frame in the main loop. Add `add_system()`, `add_resource()`, `world()` accessors.
  - **Category**: `deep` — ECS lifecycle integration is architecture-critical

- [ ] 2. **InputModule**
  - **What**: New crate `modules/input/` that:
    - Registers via linkme
    - Listens to `InputEvent` on EventBus
    - Maintains `InputState` ECS resource (keyboard bitmask `[bool; 256]`, `mouse_pos: (f32,f32)`, `mouse_buttons: [bool; 3]`)
    - Updates the resource each frame before ECS schedule runs
    - Platform events for keyboard/mouse from winit → PlatformEvent → EventBus:InputEvent chain
  - **Category**: `unspecified-high`

- [ ] 3. **Window resize in RenderModule**
  - **What**: Handle `PlatformEvent::Resized(w, h)` in the render module. Reconfigure the wgpu surface with new dimensions. Protect against zero-sized surfaces (minimized window).
  - **Category**: `quick`

### Wave 2 — Rendering Upgrades (ALL PARALLEL)

- [ ] 4. **Shader compilation pipeline**
  - **What**: Add `ShaderModule` creation from WGSL source in the render module:
    - `compile_shader(device: &Device, source: &str) -> ShaderModule`
    - `create_pipeline(device: &Device, layout: &PipelineLayout, vs: &ShaderModule, fs: &ShaderModule, config: &RenderPipelineConfig) -> RenderPipeline`
    - Support embedded WGSL (as `include_str!`) as default shaders
    - Default vertex shader + fragment shader (solid color)
  - **Category**: `unspecified-high`

- [ ] 5. **Triangle + textured quad rendering**
  - **What**: Render a triangle using the new pipeline:
    - Vertex buffer with position + UV + color
    - Index buffer
    - Render pass with actual draw call (`draw_indexed`)
    - Extend to textured quad (sample from texture in fragment shader)
    - Default white texture for untextured materials
  - **Category**: `unspecified-high`

- [ ] 6. **Basic mesh rendering**
  - **What**: Add `Mesh` struct (vertex data + index data + bounding box):
    - `Mesh::triangle()`, `Mesh::quad()`, `Mesh::cube()` factories
    - Mesh vertex format: `position: Vec3`, `normal: Vec3`, `uv: Vec2`, `color: Vec4`
    - GPU upload: create vertex buffer + index buffer from Mesh data
  - **Category**: `unspecified-high`

### Wave 3 — Assets + Systems + Demo (ALL PARALLEL)

- [ ] 7. **AssetModule with loaders**
  - **What**: New crate `modules/assets/`:
    - `AssetModule` implementing `Module` trait
    - `TextureLoader` — loads PNG via `image` crate, uploads to wgpu texture
    - `MeshLoader` — loads OBJ (simple parser) or creates primitive meshes
    - `AssetCache` — `HashMap<AssetId, Arc<AssetData>>`, reference counting
    - `load_texture(path) -> Handle<Texture>`, `load_mesh(path) -> Handle<Mesh>`
    - Embedded defaults (1×1 white texture, unit quad mesh)
  - **Category**: `unspecified-high`

- [ ] 8. **TaskSystem implementation**
  - **What**: Fill in `kernel/src/task/`:
    - `TaskSystem` wrapping bevy_ecs `ComputeTaskPool`
    - `run_schedule(world, schedule)` — execute ECS schedule with parallelism
    - Add task module to Kernel, run schedule through task system
    - `spawn(task)` for parallel work
  - **Category**: `quick`

- [ ] 9. **ECS-driven demo**
  - **What**: Create `gearx_demo` crate (or extend main.rs):
    - Add ECS systems: `input_system` (reads InputState, writes Transform), `render_system` (draws meshes)
    - Arrow keys move a quad on screen
    - Camera: orthographic projection, follow the player
    - FPS overlay in console
    - Clean exit on window close (verify event loop integration)
  - **Category**: `deep` — integration of ECS + Input + Render

### Wave 4 — QA

- [ ] 10. **Unit tests: InputModule + AssetSystem**
  -  Test InputState updates, event mapping
  -  Test AssetCache load/unload/reference counting
  -  Test Mesh factory functions (triangle, quad)

- [ ] 11. **Integration test: ECS → Input → Render**
  -  Test full pipeline: platform event → ECS resource → system execution

- [ ] 12. **Cross-compile CI update**
  -  Add new crates to workspace
  -  Update verify-phase2.sh

- [ ] 13. **Build/run verification**
  -  `cargo run` with timeout, capture output
  -  Verify FPS display, input response

### Wave FINAL — Verification (ALL PARALLEL)
- [ ] F1-F4: Same structure as Phase 1 final wave

---

## Commit Strategy

- **Q1-Q2**: `fix(kernel): address F2 code quality review findings`
- **1**: `feat(kernel): integrate bevy_ecs World and Schedule into Kernel lifecycle`
- **2**: `feat(input): add input module with keyboard and mouse support`
- **3**: `feat(render): handle window resize with wgpu surface reconfiguration`
- **4**: `feat(render): add shader compilation pipeline from WGSL source`
- **5**: `feat(render): implement triangle and textured quad rendering`
- **6**: `feat(render): add Mesh primitives and GPU upload`
- **7**: `feat(assets): add asset module with texture and mesh loaders`
- **8**: `feat(kernel): implement TaskSystem with ComputeTaskPool`
- **9**: `feat(demo): add ECS-driven demo with keyboard movement`
- **10-11**: `test: add InputModule, AssetSystem, and integration tests`
- **12-13**: `ci: update cross-compile and build verification for Phase 2`

---

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|-----------|
| wgpu surface resize crashes | Medium | High | Test resize extensively; handle zero-size gracefully |
| winit 0.30 event pump missing on Wayland | Low | High | X11 fallback; test on both X11 and Wayland |
| bevy_ecs version conflict | Low | Medium | Pin bevy_ecs 0.15; verify Schedule API compatibility |
| Asset loading slows startup | Medium | Low | Async loading deferred to Phase 3; embedded defaults are instant |
| F2 transmute fix breaks event loop | Medium | Medium | Test event loop behavior after refactor; have rollback plan |
