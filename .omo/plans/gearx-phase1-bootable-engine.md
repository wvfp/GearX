# GearX Phase 1 — Bootable Engine

## TL;DR

> **Quick Summary**: Build the minimal bootable foundation of GearX — a Cargo workspace containing the kernel crate (module system, ECS, event bus, platform abstraction) and a render module stub, producing a desktop binary that opens a winit window with wgpu clear color. Android and Web platform stubs compile but are no-ops.
>
> **Deliverables**:
> - Root Cargo workspace with `.cargo/config.toml`
> - `gearx_kernel` library crate (Module trait, linkme registry, EventBus, Platform trait, Kernel lifecycle)
> - `gearx_render` library crate (wgpu device/surface init + clear color render)
> - Desktop binary `gearx` (winit window + wgpu surface)
> - Android platform stub (android_activity entry point)
> - Web platform stub (wasm_bindgen entry point)
> - Unit tests + integration test + build verification script
>
> **Estimated Effort**: Medium (16 implementation tasks + 4 verification)
> **Parallel Execution**: YES — 4 waves, max 5 concurrent tasks
> **Critical Path**: Task 2 (kernel crate) → Task 4/5/6 (Module + Platform + EventBus) → Task 8 (Kernel struct) → Task 10 (desktop binary) → Wave F1-F4 → user approval

---

## Context

### Original Request
Build a modular 2D/3D game engine named GearX from scratch, with a Linux-like kernel architecture, built-in AI Agent, on-device Android editor, and full cross-platform support (Windows/Linux/Android/Web). Phase 1 is the minimal bootable engine.

### Interview Summary
**Key Discussions**:
- **Architecture**: Full 19-subsystem architecture documented at `.omo/plans/gearx-architecture.md` (1561 lines, 7 layers, 4 platforms)
- **Phase 1 scope**: Confirmed as "Bootable Engine" — kernel + desktop window + platform stubs. No editor, no physics, no scripting, no AI Agent yet.
- **Platform priority**: Linux x86_64 first (desktop dev/run). Android/Web: compile-check only (no executable builds).
- **Rust channel**: Stable only. No `#![feature(...)]`.
- **Module registration**: linkme >=0.3.32 (RUSTSEC-2024-0407 patched).
- **Error handling**: anyhow. **Logging**: tracing + tracing-subscriber.

**Research Findings**:
- linkme 0.3 works on stable Rust (1.66+) via `#[link_section]` — not nightly-only
- wgpu surface creation requires `raw_window_handle::HasWindowHandle` — Platform::Window must implement this
- bevy_ecs standalone includes Schedule + Events + World — can serve as both ECS and (partial) event system
- winit 0.30+ uses `EventLoop::run()` (push model) — on Linux this blocks; for non-blocking polling use `EventLoopExtRunOnDemand` (requires `rwh_06` feature on winit)
- Android: requires `android_activity` crate + NDK + proper `#[no_mangle]` JNI entry points
- Web: requires `wasm-bindgen` + `web-sys` + canvas element access

### Metis Review
**Identified Gaps** (all addressed):
- **BLOCKER**: Platform↔wgpu surface coupling — resolved: `Window` trait will implement `HasWindowHandle` from `raw_window_handle`
- **BLOCKER**: Android stub requires android_activity — resolved: added as dependency, task specifies android_main() entry point
- **BLOCKER**: Web stub requires wasm-bindgen — resolved: added as dependency, task specifies `#[wasm_bindgen(start)]` entry point
- **Scope creep**: TaskSystem deferred to Phase 2
- **Scope creep**: Topological sort deferred to Phase 2 (simple Vec in Phase 1)
- **Scope creep**: EventBus async queue deferred to Phase 2 (sync-only in Phase 1)
- **Scope creep**: Platform FileSystem + Clipboard deferred (default empty methods)
- **Premature**: EngineMode editor logic — set_mode() is a simple stub
- **Security**: linkme locked to >=0.3.32 (RUSTSEC-2024-0407)

---

## Work Objectives

### Core Objective
Build a single bootable desktop binary that initializes the GearX kernel, discovers the render module via linkme, opens a winit window, clears it with a solid color via wgpu, and exits cleanly on close. Android and Web targets compile-check but produce no executable.

### Concrete Deliverables
1. Cargo workspace at `/root/GearX/Cargo.toml` with 3 members: root binary, kernel lib, render lib
2. `gearx_kernel` crate: Module trait, linkme registry, EventBus, Platform abstraction, Kernel struct
3. `gearx_render` crate: wgpu device init, surface creation, clear-color frame loop stub
4. Desktop binary `gearx` in `src/main.rs`: boots Kernel, opens window, runs until close
5. `build/android/` — AndroidManifest.xml + platform stub `.rs`
6. `build/web/` — Web platform stub `.rs`
7. Unit tests (EventBus, ModuleRegistry) + integration test (boot→frame→shutdown)
8. `scripts/verify-phase1.sh` — automated verification script

### Definition of Done
- [x] `cargo build --workspace` succeeds on Linux x86_64 (stable Rust)
- [~] `cargo run` opens window titled "GearX" with solid color background — BLOCKED: headless CI, no display
- [~] Console output (stderr) shows "GearX Kernel v0.1.0 initialized" and "Module loaded: render" — BLOCKED: needs display to verify
- [~] Engine exits cleanly on window close (no panic, no hang) — BLOCKED: needs display to verify
- [x] `cargo test --workspace` — all unit + integration tests pass
- [~] `cargo check --target wasm32-unknown-unknown` succeeds — BLOCKED: wasm target not installed
- [~] `cargo check --target aarch64-linux-android` succeeds — BLOCKED: Android NDK not installed
- [x] No `#![feature(...)]` in any `.rs` file

### Must Have
- Module trait + linkme distributed slice + ModuleRegistry (simple Vec init order)
- Platform trait with WindowSystem (create_window, run_event_loop, raw_window_handle) + TimeSystem (now)
- Desktop platform impl using winit + raw-window-handle
- EventBus with sync-only publish (TypeId-HashMap channels)
- Kernel struct with new()/run()/shutdown()/set_mode() lifecycle
- Render module that initializes wgpu Instance→Adapter→Device→Queue→Surface and clears with a solid color each frame
- Desktop binary that boots Kernel and runs until CloseRequested
- tracing + tracing-subscriber for logging (stderr output)
- anyhow for error handling (all fallible functions return anyhow::Result)
- Unit tests for EventBus and ModuleRegistry
- Integration test: Kernel boot → 1+ frames → shutdown
- Platform stubs for Android and Web (conditionally compiled)
- linkme >= 0.3.32 (locked)

### Must NOT Have (Guardrails)
- NO editor UI of any kind
- NO input processing (beyond winit default CloseRequested)
- NO asset loading / file I/O
- NO physics, audio, network, AI Agent, scripting, animation, particles, VFX, navigation, AI behavior
- NO TaskSystem (deferred to Phase 2)
- NO topological sort in ModuleRegistry (simple Vec only)
- NO async EventBus queue / flush (sync publish only)
- NO EngineMode switching logic (set_mode is a stub)
- NO profiler beyond a basic frame counter
- NO ResourceManager loading/caching/watching (empty struct)
- NO shader compilation or pipeline creation in render module
- NO render graph in render module
- NO Windows/macOS requirements (Linux x86_64 is Phase 1 target)
- NO Android APK build or Web WASM binary — compile-check only
- NO `#![feature(...)]` — stable Rust only

---

## Verification Strategy (MANDATORY)

> **ZERO HUMAN INTERVENTION** — ALL verification is agent-executed. No exceptions.

### Test Decision
- **Infrastructure exists**: NO (greenfield project)
- **Automated tests**: Tests-after (unit + integration)
- **Framework**: `cargo test` (Rust built-in)
- **Agent QA**: Mandatory for ALL tasks — agent-executable bash assertions

### QA Policy
Every task MUST include agent-executable QA scenarios. Evidence saved to `.omo/evidence/task-{N}-{scenario-slug}.{ext}`.

- **Build verification**: `bash` — `cargo build --workspace`, check exit code + output
- **Runtime verification**: `bash` — `timeout 3 cargo run 2>&1`, check stdout/stderr for expected strings
- **Test verification**: `bash` — `cargo test --workspace 2>&1`, check "PASS" / test count
- **Cross-compile**: `bash` — `cargo check --target {triple} 2>&1`, check exit code
- **Pattern enforcement**: `grep` — search for forbidden patterns (e.g. `#![feature`)
- **Code review**: `ast_grep_search` / `grep` for AI slop patterns

---

## Execution Strategy

### Parallel Execution Waves

```
Wave 1 (Start Immediately — 3 tasks, ALL PARALLEL):
├── Task 1: Root workspace + config files [quick]
├── Task 2: kernel crate scaffolding [quick]
└── Task 3: modules/render crate scaffolding [quick]

Wave 2 (After Wave 1 — 5 tasks, 4 PARALLEL + 1 SEQUENTIAL):
├── Task 4: Module trait + linkme + ModuleRegistry [quick]          ╮
├── Task 5: Platform trait + desktop (winit) impl [unspecified]     ├── ALL PARALLEL
├── Task 6: EventBus (sync publish) [quick]                         ╯
├── Task 7: Tracing logger + frame counter [quick]
└── Task 8: Kernel struct + lifecycle (depends on 4, 5, 6) [deep]

Wave 3 (After Wave 2 — 4 tasks, ALL PARALLEL):
├── Task 9: RenderModule stub (wgpu init + clear color) [unspecified]
├── Task 10: Desktop binary src/main.rs [quick]
├── Task 11: Android platform stub [quick]
└── Task 12: Web platform stub [quick]

Wave 4 (After Wave 3 — 4 tasks, ALL PARALLEL):
├── Task 13: EventBus unit tests [quick]
├── Task 14: ModuleRegistry unit tests [quick]
├── Task 15: Integration test — boot→frame→shutdown [unspecified]
└── Task 16: Cross-compile CI script + build verification [quick]

Wave FINAL (After ALL tasks — 4 reviews, ALL PARALLEL):
├── Task F1: Plan compliance audit (oracle)
├── Task F2: Code quality review (unspecified-high)
├── Task F3: Real manual QA (unspecified-high + bash)
└── Task F4: Scope fidelity check (deep)
-> Present results -> Get explicit user okay

Critical Path: 2 → 4/5/6 → 8 → 10 → F1-F4 → user okay
Parallel Speedup: ~65% faster than sequential
Max Concurrent: 5 (Wave 2)
```

### Dependency Matrix

| Task | Depends On | Blocks |
|------|-----------|--------|
| 1 | — | 2, 3 |
| 2 | 1 | 4, 5, 6, 7 |
| 3 | 1 | 9 |
| 4 | 2 | 8 |
| 5 | 2 | 8, 10, 11, 12 |
| 6 | 2 | 8 |
| 7 | 2 | (none — logging used by all) |
| 8 | 4, 5, 6 | 9, 10 |
| 9 | 3, 8 | 15, F3 |
| 10 | 8 | 15 |
| 11 | 5 | 16 |
| 12 | 5 | 16 |
| 13 | 6 | 15 |
| 14 | 4 | 15 |
| 15 | 10, 13, 14 | F1-F4 |
| 16 | 11, 12 | (none — verification only) |
| F1 | All | (none — parallel with F2, F3, F4) |
| F2 | All | User OK |
| F3 | All | User OK |
| F4 | All | User OK |

### Agent Dispatch Summary

- **Wave 1**: 3× quick
- **Wave 2**: 3× quick, 1× unspecified-high (Platform), 1× deep (Kernel struct)
- **Wave 3**: 1× unspecified-high (Render), 3× quick
- **Wave 4**: 3× quick, 1× unspecified-high (Integration test)
- **Wave FINAL**: 1× oracle, 1× unspecified-high (Code review), 1× unspecified-high (QA), 1× deep (Scope fidelity)

---

## TODOs

### Wave 1 — Foundation (ALL PARALLEL)

- [x] 1. **Root workspace Cargo.toml + config files**

  **What to do**:
  - Create `/root/GearX/Cargo.toml` with `[workspace]` + `[package]` members: `"."`, `"kernel"`, `"modules/render"`. Set `resolver = "2"`, `edition = "2021"`. Root package is binary `gearx` with dep on `gearx_kernel` and `gearx_render`.
  - Create `/root/GearX/.cargo/config.toml` with `[target.wasm32-unknown-unknown]` rustflags for WASM linking (if needed) and `[target.aarch64-linux-android]` linker config.
  - Create `/root/GearX/rustfmt.toml` with project defaults (tab_spaces = 4, edition = "2021", etc.).
  - Create `/root/GearX/build/android/AndroidManifest.xml` with minimal Activity declaration.
  - Create `/root/GearX/scripts/` directory placeholder.

  **Must NOT do**:
  - Do NOT add any module features or workspace-level dep overrides yet.
  - Do NOT add Windows/macOS specific config — Linux x86_64 only.

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: none needed (straightforward file creation)

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 2, 3)
  - **Parallel Group**: Wave 1 (with Tasks 2, 3)
  - **Blocks**: Tasks 2, 3
  - **Blocked By**: None

  **References**:
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 205-229 (workspace members list)
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 806-839 (feature gate pattern — for reference, not needed in Phase 1)

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: Workspace resolves all members
    Tool: bash
    Steps:
      1. Run `cargo metadata --format-version=1 --no-deps 2>&1 | python3 -c "import sys,json; d=json.load(sys.stdin); print([p['name'] for p in d['packages']])"`
    Expected Result: Packages list includes "gearx", "gearx_kernel", "gearx_render"
    Evidence: .omo/evidence/task-1-workspace-members.txt

  Scenario: Config files exist
    Tool: bash
    Steps:
      1. `ls -la .cargo/config.toml rustfmt.toml build/android/AndroidManifest.xml`
    Expected Result: All 3 files exist and are non-empty
    Evidence: .omo/evidence/task-1-config-files.txt
  ```

  **Commit**: YES
  - Message: `chore(workspace): initialize Cargo workspace with root, kernel, render crates`
  - Files: `Cargo.toml`, `.cargo/config.toml`, `rustfmt.toml`, `build/android/AndroidManifest.xml`

- [x] 2. **kernel crate scaffolding (gearx_kernel)**

  **What to do**:
  - Create `/root/GearX/kernel/Cargo.toml` with deps: `bevy_ecs` (default features minimal), `linkme` (>=0.3.32), `glam`, `raw-window-handle` (0.6), `tracing`, `anyhow`. Name = `gearx_kernel`, version = `0.1.0`, edition = `2021`.
  - Create directory structure under `kernel/src/`:
    - `lib.rs` — top-level module declarations
    - `module/mod.rs` — Module trait, MODULES distributed slice, ModuleRegistry
    - `ecs/mod.rs` — bevy_ecs re-exports, kernel resources placeholder
    - `event/mod.rs` — EventBus, EventHandler trait, event types
    - `task/mod.rs` — empty placeholder (deferred to Phase 2)
    - `resource/mod.rs` — empty placeholder (deferred to Phase 2)
    - `math/mod.rs` — glam re-exports
    - `platform/mod.rs` — Platform trait (WindowSystem + TimeSystem)
    - `profiler/mod.rs` — empty placeholder
    - `console/mod.rs` — empty placeholder

  **Must NOT do**:
  - Do NOT implement any logic yet — only crate structure + mod.rs skeletons
  - Do NOT add feature gates (deferred to Phase 2)
  - Do NOT implement task/resource/profiler/console — empty `mod.rs` only

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: none needed (file/directory creation + Cargo.toml)

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 1, 3)
  - **Parallel Group**: Wave 1 (with Tasks 1, 3)
  - **Blocks**: Tasks 4, 5, 6, 7
  - **Blocked By**: Task 1

  **References**:
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 241-273 (kernel directory structure)
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 461-495 (Kernel struct fields — to guide which modules to scaffold)

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: Kernel crate structure is correct
    Tool: bash
    Steps:
      1. Run `ls -R kernel/src/`
    Expected Result: All subdirectories exist: module, ecs, event, task, resource, math, platform, profiler, console
    Evidence: .omo/evidence/task-2-kernel-structure.txt

  Scenario: Kernel crate compiles as library
    Tool: bash
    Steps:
      1. Run `cargo build -p gearx_kernel 2>&1`
    Expected Result: Exit code 0. Output shows "Compiling gearx_kernel v0.1.0"
    Evidence: .omo/evidence/task-2-kernel-build.txt
  ```

  **Commit**: YES (groups with Task 3)
  - Message: `feat(kernel): scaffold gearx_kernel crate with module structure`
  - Files: `kernel/Cargo.toml`, `kernel/src/**/*`

- [x] 3. **modules/render crate scaffolding (gearx_render)**

  **What to do**:
  - Create `/root/GearX/modules/render/Cargo.toml` with deps: `gearx_kernel` (path dep), `wgpu` (24.x+), `winit` (0.30.x+ with `rwh_06` feature), `raw-window-handle` (0.6), `tracing`, `anyhow`. Name = `gearx_render`, version = `0.1.0`, edition = `2021`.
  - Create `/root/GearX/modules/render/src/lib.rs` with:
    - `RenderModule` struct declaration (empty fields for now)
    - `impl Module for RenderModule` skeleton (trait methods with `todo!()`)
    - linkme distributed slice registration: `#[distributed_slice(MODULES)] fn register(...)`

  **Must NOT do**:
  - Do NOT implement wgpu init here — task 9 does that
  - Do NOT add shaders, pipelines, or any rendering logic

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: none needed

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 1, 2)
  - **Parallel Group**: Wave 1 (with Tasks 1, 2)
  - **Blocks**: Task 9
  - **Blocked By**: Task 1

  **References**:
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 275-287 (render module directory — Phase 1 only needs lib.rs)
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 789-799 (linkme registration pattern)

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: Render crate compiles as library
    Tool: bash
    Steps:
      1. Run `cargo build -p gearx_render 2>&1`
    Expected Result: Exit code 0. Output shows "Compiling gearx_render v0.1.0"
    Evidence: .omo/evidence/task-3-render-build.txt

  Scenario: linkme registration exists
    Tool: ast_grep_search
    Steps:
      1. Pattern: `distributed_slice(MODULES)` lang: rust in modules/render/
    Expected Result: Exactly 1 match — the module registration function
    Evidence: .omo/evidence/task-3-linkme-registration.txt
  ```

  **Commit**: YES (groups with Task 2)
  - Message: `feat(render): scaffold gearx_render crate with Module trait skeleton`
  - Files: `modules/render/Cargo.toml`, `modules/render/src/lib.rs`

---

### Wave 2 — Kernel Core (4 PARALLEL + 1 SEQUENTIAL)

- [x] 4. **Module trait + linkme + ModuleRegistry**

  **What to do**:
  - Implement `Module` trait in `kernel/src/module/mod.rs`:
    - Required methods: `name() -> &'static str`, `init(&mut self, kernel: &mut Kernel) -> Result<()>`, `update(&mut self, kernel: &mut Kernel, dt: f32) -> Result<()>`, `shutdown(&mut self, kernel: &mut Kernel) -> Result<()>`
    - Default methods: `dependencies() -> &[&'static str] { &[] }`, `priority() -> u32 { 0 }`
  - Define `#[distributed_slice] pub static MODULES: [fn() -> Box<dyn Module>]` (factory signature is `fn() -> Box<dyn Module>`, NOT `fn(&mut Kernel)` — avoids dual-init path)
  - Implement `ModuleRegistry` in `kernel/src/module/registry.rs`:
    - Fields: `entries: Vec<ModuleEntry>` where `ModuleEntry { module: Box<dyn Module>, state: ModuleState }`
    - `new()` — empty registry
    - `register(module: Box<dyn Module>)` — push to vec
    - `discover()` — iterate `MODULES` slice, call each factory, call `register()`
    - `load_all(kernel: &mut Kernel)` — iterate `entries` in order, call `init()` for each. If any init fails, collect error and shutdown already-initialized modules.
    - `unload_all(kernel: &mut Kernel)` — iterate in reverse, call `shutdown()`
    - `get(name: &str) -> Option<&Box<dyn Module>>` — find by name
    - `iter_mut()` — mutable iterator for the main loop

  **Must NOT do**:
  - NO topological sort (simple Vec init order)
  - NO dependency resolution or cycle detection
  - NO module priority sorting

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: none needed (standard Rust trait + struct implementation)

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 5, 6, 7)
  - **Parallel Group**: Wave 2 (with Tasks 5, 6, 7)
  - **Blocks**: Task 8
  - **Blocked By**: Task 2

  **References**:
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 574-636 (Module trait, distributed slice, ModuleRegistry)
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 789-799 (linkme registration pattern in module crates)

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: Module trait compiles with correct signature
    Tool: bash
    Steps:
      1. Run `cargo build -p gearx_kernel 2>&1`
    Expected Result: Compiles. No errors about Module trait.
    Evidence: .omo/evidence/task-4-module-trait-compile.txt

  Scenario: Factory signature is fn() -> Box<dyn Module> (not &mut Kernel)
    Tool: ast_grep_search
    Steps:
      1. Pattern: `pub static MODULES` lang: rust in kernel/src/
      2. Pattern: `fn() -> Box<dyn Module>` lang: rust in kernel/src/
    Expected Result: MODULES distributed slice declared with fn() -> Box<dyn Module> signature
    Evidence: .omo/evidence/task-4-factory-signature.txt
  ```

  **Evidence to Capture**:
  - [ ] Module trait file contents (kernel/src/module/mod.rs)
  - [ ] Registry file contents (kernel/src/module/registry.rs)

  **Commit**: NO (groups with Task 8)

- [x] 5. **Platform trait + desktop (winit) implementation**

  **What to do**:
  - Implement `Platform` trait in `kernel/src/platform/mod.rs`:
    - Required: `name() -> &'static str`
    - Required: `window_system() -> &dyn WindowSystem`
    - Required: `time_system() -> &dyn TimeSystem`
    - Default methods (empty): `file_system() -> Option<&dyn FileSystem> { None }`, `clipboard() -> Option<&dyn Clipboard> { None }`
  - Implement `WindowSystem` trait:
    - `create_window(title: &str, width: u32, height: u32) -> Box<dyn Window>`
    - `run_event_loop(&mut self, on_event: &mut dyn FnMut(PlatformEvent))`
  - Implement `Window` trait:
    - `raw_window_handle() -> raw_window_handle::WindowHandle` — **critical** for wgpu surface creation
    - `size() -> (u32, u32)`
  - Implement `TimeSystem` trait:
    - `now() -> Duration`
    - `delta_seconds() -> f32`
  - Implement `PlatformEvent` enum:
    - `CloseRequested`, `Resized(u32, u32)`, `Tick(f32)`
  - Implement desktop (winit) backend in `kernel/src/platform/desktop.rs`:
    - `DesktopPlatform` struct wrapping `EventLoop<Tick>`
    - `DesktopWindow` wrapping `winit::window::Window` that implements `HasWindowHandle` for `raw_window_handle()`
    - `DesktopTime` using `std::time::Instant`
    - Event loop: use `EventLoopExtRunOnDemand` for non-blocking polling (available with `rwh_06` feature on winit 0.30+). Map winit events to `PlatformEvent`.
  - Create `kernel/src/platform/android.rs` — `AndroidPlatform` stub with `todo!()`
  - Create `kernel/src/platform/web.rs` — `WebPlatform` stub with `todo!()`

  **Must NOT do**:
  - NO filesystem operations
  - NO clipboard access
  - NO multi-window support (single window only)
  - Do NOT implement headless mode yet

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: none needed
  - **Reason**: winit event loop integration requires careful API usage (poll vs push model, rwh_06 feature gate)

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 4, 6, 7)
  - **Parallel Group**: Wave 2 (with Tasks 4, 6, 7)
  - **Blocks**: Tasks 8, 10, 11, 12
  - **Blocked By**: Task 2

  **References**:
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 162-190 (Platform trait + sub-traits)
  - `winit` docs: EventLoopExtRunOnDemand for non-blocking polling API
  - `raw_window_handle` crate: `HasWindowHandle` trait implementation on winit::window::Window

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: Platform trait compiles
    Tool: bash
    Steps:
      1. Run `cargo build -p gearx_kernel 2>&1`
    Expected Result: Compiles. Platform trait, WindowSystem, TimeSystem all defined.
    Evidence: .omo/evidence/task-5-platform-compile.txt

  Scenario: DesktopWindow implements HasWindowHandle
    Tool: ast_grep_search
    Steps:
      1. Pattern: `impl HasWindowHandle for DesktopWindow` lang: rust in kernel/src/platform/
    Expected Result: Exactly 1 match
    Evidence: .omo/evidence/task-5-raw-window-handle.txt
  ```

  **Evidence to Capture**:
  - [ ] Platform trait file (kernel/src/platform/mod.rs)
  - [ ] Desktop impl file (kernel/src/platform/desktop.rs)

  **Commit**: NO (groups with Task 8)

- [x] 6. **EventBus (sync publish)**

  **What to do**:
  - Implement `EventBus` in `kernel/src/event/mod.rs`:
    - Store: `HashMap<TypeId, Vec<Box<dyn AnyHandler>>>` — channel-per-type, sync dispatch only. No async queue, no `flush()`.
    - `listen<E: Event, H: EventHandler<E>>(&mut self, handler: H)` — register handler for event type. Wrap in `AnyHandler` box.
    - `publish<E: Event>(&mut self, event: &E)` — look up handlers by TypeId, call each handler's `handle(event)`. Publish is **synchronous** — handlers run immediately.
    - `remove_all<E: Event>(&mut self)` — clear handlers for type
    - Thread safety: EventBus should be `Send + Sync` (use `Mutex<HashMap<...>>` or require external synchronization — for Phase 1, it's used from the main thread only, but design should not prevent Send)
  - Define `Event` trait: `pub trait Event: Send + 'static {}`
  - Define `EventHandler` trait: `pub trait EventHandler<E: Event>: Send + 'static { fn handle(&mut self, event: &E); }`
  - Implement internal `AnyHandler` trait for type erasure
  - Define kernel pre-defined events:
    - `EngineEvent::ModulesLoaded`, `EngineEvent::Shutdown`, `EngineEvent::ModeChanged(EngineMode)`
    - `FrameEvent::Start(f32)`, `FrameEvent::End(f32)`
    - `InputEvent` — placeholder enum (empty variants for now)
  - Re-export all types from `kernel/src/lib.rs`

  **Must NOT do**:
  - NO async send queue
  - NO flush() method
  - NO cross-thread channels or mpsc
  - NO bevy_ecs event integration yet (Phase 2 concern)
  - Register NO built-in event handlers (Kernel will do that)

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: none needed (standard event bus pattern)

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 4, 5, 7)
  - **Parallel Group**: Wave 2 (with Tasks 4, 5, 7)
  - **Blocks**: Task 8
  - **Blocked By**: Task 2

  **References**:
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 638-704 (EventBus design, event type definitions)

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: EventBus compiles with correct API
    Tool: bash
    Steps:
      1. Run `cargo build -p gearx_kernel 2>&1`
    Expected Result: Compiles. EventBus with listen/publish methods.
    Evidence: .omo/evidence/task-6-eventbus-compile.txt

  Scenario: Pre-defined events exist
    Tool: ast_grep_search
    Steps:
      1. Pattern: `pub enum EngineEvent` lang: rust in kernel/src/event/
      2. Pattern: `pub enum FrameEvent` lang: rust in kernel/src/event/
    Expected Result: Both enums defined
    Evidence: .omo/evidence/task-6-events-exist.txt
  ```

  **Commit**: NO (groups with Task 8)

- [x] 7. **Tracing logger + frame counter**

  **What to do**:
  - Create `/root/GearX/kernel/src/init.rs`:
    - `pub fn init_logging()` — call `tracing_subscriber::fmt::init()` with default settings (stderr output, timestamp, level filter from `RUST_LOG` env var)
    - `pub fn init_logging_with_default(level: &str)` — same but with fallback level
  - Create `/root/GearX/kernel/src/frame_counter.rs`:
    - `FrameCounter` struct: `frame_count: u64`, `last_log: Instant`, `fps: f32`
    - `tick()` — increment count, recalculate FPS, `trace!("Frame {count} complete")`
  - Wire into kernel: KERNEL_VERSION constant, startup banner log

  **Must NOT do**:
  - NO structured JSON logging
  - NO file appender
  - NO Console UI integration (Phase 2+)
  - NO tracing spans or instrumentation yet (just basic logging)

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: none needed

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 4, 5, 6)
  - **Parallel Group**: Wave 2 (with Tasks 4, 5, 6)
  - **Blocks**: None (independent utility)
  - **Blocked By**: Task 2

  **References**:
  - tracing crate docs: `https://docs.rs/tracing/latest/tracing/`
  - tracing-subscriber docs: `https://docs.rs/tracing-subscriber/latest/tracing_subscriber/`

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: Logging compiles and no-op on default
    Tool: bash
    Steps:
      1. Run `cargo build -p gearx_kernel 2>&1`
    Expected Result: Compiles without errors
    Evidence: .omo/evidence/task-7-logging-compile.txt

  Scenario: FrameCounter compiles
    Tool: bash
    Steps:
      1. Run `cargo build -p gearx_kernel 2>&1`
    Expected Result: Compiles
    Evidence: .omo/evidence/task-7-framecounter-compile.txt
  ```

  **Commit**: NO (groups with Task 8)

- [x] 8. **Kernel struct + lifecycle (new/run/shutdown/set_mode)**

  **What to do**:
  - Implement `Kernel` struct in `kernel/src/lib.rs`:
    - Fields: `world: World` (bevy_ecs), `modules: ModuleRegistry`, `event_bus: EventBus`, `schedule: Schedule` (bevy_ecs), `platform: Box<dyn Platform>`, `frame_counter: FrameCounter`, `mode: EngineMode`, `running: bool`
  - Implement `Kernel::new(platform: Box<dyn Platform>) -> Self`:
    - Create World, insert `Time` and `FrameStats` resources
    - Call `ModuleRegistry::discover()` to populate from linkme slice
    - Init frame counter
    - Log: `info!("GearX Kernel v{} initialized", KERNEL_VERSION)`
  - Implement `Kernel::run(&mut self) -> Result<()>`:
    - `self.modules.load_all(self)?` — calls init() on each module
    - Log: `info!("Loaded {} modules", count)` — including names
    - Main loop: `while self.running { ... }`:
      - `self.platform.time_system().delta_seconds()` → dt
      - `self.event_bus.publish(&FrameEvent::Start(dt))`
      - Iterate modules: `module.update(self, dt)?`
      - Run ECS schedule: `self.schedule.run(&mut self.world)`
      - Update frame counter
      - `self.event_bus.publish(&FrameEvent::End(dt))`
      - Poll platform events: if `CloseRequested`, set `running = false`
    - After loop: `self.modules.unload_all(self)?`
    - Log: `info!("GearX Kernel shutdown complete")`
  - Implement `Kernel::set_mode(&mut self, mode: EngineMode)`:
    - Store mode, publish `EngineEvent::ModeChanged(mode)` (no editor logic)
  - Implement `EngineMode` enum: `Edit`, `Play`, `Pause`
  - Wire up platform event handler in `Kernel::run()`: subscribe to `PlatformEvent::CloseRequested` → stop loop

  **Must NOT do**:
  - NO editor/play mode switching logic
  - NO multi-threading (main thread only)
  - NO integration with render module here (render registers itself via linkme)

  **Recommended Agent Profile**:
  - **Category**: `deep`
  - **Skills**: none needed
  - **Reason**: Kernel is the central integration point — must correctly wire ModuleRegistry, EventBus, Platform, and Schedule together. Lifecycle correctness is critical.

  **Parallelization**:
  - **Can Run In Parallel**: NO (depends on Tasks 4, 5, 6)
  - **Parallel Group**: Sequential (after Wave 2 parallel set)
  - **Blocks**: Tasks 9, 10
  - **Blocked By**: Tasks 4, 5, 6

  **References**:
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 459-569 (Kernel struct + lifecycle + run loop)
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 1137-1160 (set_mode + EngineMode)

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: Kernel crate builds with all dependencies
    Tool: bash
    Steps:
      1. Run `cargo build -p gearx_kernel 2>&1`
    Expected Result: Exit code 0. No errors.
    Evidence: .omo/evidence/task-8-kernel-build.txt

  Scenario: Kernel new() signature is correct
    Tool: ast_grep_search
    Steps:
      1. Pattern: `pub fn new(platform: Box<dyn Platform>) -> Self` lang: rust in kernel/src/lib.rs
    Expected Result: Exactly 1 match with correct signature
    Evidence: .omo/evidence/task-8-new-signature.txt
  ```

  **Evidence to Capture**:
  - [ ] Full Kernel source (kernel/src/lib.rs)

  **Commit**: YES
  - Message: `feat(kernel): implement Module system, Platform, EventBus, and Kernel lifecycle`
  - Files: `kernel/src/**/*`
  - Pre-commit: `cargo build -p gearx_kernel`

---

### Wave 3 — Runnable Outputs (ALL PARALLEL)

- [x] 9. **RenderModule stub (wgpu init + clear color)**

  **What to do**:
  - Implement `RenderModule` in `modules/render/src/lib.rs`:
    - Fields: `instance: wgpu::Instance`, `surface: wgpu::Surface<'static>`, `device: wgpu::Device`, `queue: wgpu::Queue`, `surface_config: wgpu::SurfaceConfiguration`, `clear_color: wgpu::Color`
  - `impl Module for RenderModule`:
    - `name() -> "render"`
    - `init(&mut self, kernel: &mut Kernel)`:
      1. Get window handle from `kernel.platform.window_system()` — call `create_window("GearX", 1280, 720)`
      2. Create `wgpu::Instance` (default descriptor)
      3. Create `wgpu::Surface` from the window's raw handle
      4. Request adapter: `instance.request_adapter(...)` with `wgpu::PowerPreference::HighPerformance`
      5. Request device: `adapter.request_device(...)` with default features/limits
      6. Configure surface: `surface.configure(&device, &config)` with preferred format, 1280x720, PresentMode::AutoVsync
      7. Set `clear_color = wgpu::Color::BLUE` (any noticeable color)
      8. Register `ClearColor` resource with kernel.world
      9. Log: `info!("Render module initialized: {}x{} on {}", w, h, adapter_info.name)`
    - `update(&mut self, kernel: &mut Kernel, _dt: f32)`:
      1. Get current frame: `self.surface.get_current_texture()?`
      2. Create `TextureView` from current texture
      3. Create `CommandEncoder`, begin render pass with `clear_color` (wgpu::LoadOp::Clear)
      4. Drop render pass (no draw calls)
      5. Submit command encoder
      6. Present: `texture.present()`
      7. Log: `trace!("Frame rendered")`
    - `shutdown(&mut self, _kernel: &mut Kernel)`:
      - Drop wgpu resources gracefully (wgpu handles this on drop)
  - Ensure linkme registration: `#[distributed_slice(gearx_kernel::MODULES)] fn register_render(kernel: &mut Kernel) -> Box<dyn Module>`

  **Must NOT do**:
  - NO shader compilation or loading
  - NO pipeline creation
  - NO render graph
  - NO ECS rendering systems
  - NO swapchain re-creation on resize (just use fixed 1280x720)
  - NO multi-sampling or MSAA

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: none needed
  - **Reason**: wgpu device/surface init is ~80-120 lines of boilerplate with specific API ordering (Instance→Adapter→Device→Surface→Config). Must handle surface creation from raw window handle correctly.

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 10, 11, 12)
  - **Parallel Group**: Wave 3 (with Tasks 10, 11, 12)
  - **Blocks**: Task 15 (integration test), Task F3 (manual QA)
  - **Blocked By**: Tasks 3, 8

  **References**:
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 1167-1216 (Render module design, render pipeline flow)
  - wgpu docs: `https://docs.rs/wgpu/latest/wgpu/` — Instance::new, Surface creation from raw handle, adapter/device request
  - raw-window-handle: `HasWindowHandle` trait for wgpu surface creation

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: RenderModule registers correctly via linkme
    Tool: bash
    Steps:
      1. Run `cargo build -p gearx_render 2>&1`
    Expected Result: Compiles. linkme distributed_slice registration present.
    Evidence: .omo/evidence/task-9-render-linkme.txt

  Scenario: Render module compiles with wgpu deps
    Tool: bash
    Steps:
      1. Run `cargo build -p gearx_render 2>&1`
    Expected Result: No compilation errors. wgpu types used correctly.
    Evidence: .omo/evidence/task-9-render-wgpu-compile.txt

  Scenario: Module name returns "render"
    Tool: ast_grep_search
    Steps:
      1. Pattern: `fn name(&self) -> &'static str { "render" }` lang: rust in modules/render/src/lib.rs
    Expected Result: Exactly 1 match
    Evidence: .omo/evidence/task-9-render-name.txt
  ```

  **Evidence to Capture**:
  - [ ] RenderModule full source

  **Commit**: YES
  - Message: `feat(render): implement RenderModule with wgpu device init and clear color`
  - Files: `modules/render/src/lib.rs`
  - Pre-commit: `cargo build -p gearx_render`

- [x] 10. **Desktop binary src/main.rs**

  **What to do**:
  - Create `/root/GearX/src/main.rs`:
    ```rust
    fn main() -> anyhow::Result<()> {
        // 1. Init logging
        gearx_kernel::init_logging_with_default("info");

        // 2. Create platform
        let platform = gearx_kernel::platform::desktop::DesktopPlatform::new()?;

        // 3. Create and run kernel
        let mut kernel = gearx_kernel::Kernel::new(Box::new(platform));
        kernel.run()?;

        Ok(())
    }
    ```
  - Ensure root `Cargo.toml` has `[[bin]]` section: `name = "gearx"`, `path = "src/main.rs"`

  **Must NOT do**:
  - NO argument parsing (deferred)
  - NO config file loading
  - NO editor activation
  - NO Ctrl+C handler (Phase 2)

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: none needed

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 9, 11, 12)
  - **Parallel Group**: Wave 3 (with Tasks 9, 11, 12)
  - **Blocks**: Task 15 (integration test)
  - **Blocked By**: Task 8

  **References**:
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 459-569 (Kernel lifecycle — binary calls new then run)

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: Desktop binary compiles
    Tool: bash
    Steps:
      1. Run `cargo build -p gearx 2>&1`
    Expected Result: Exit code 0. Compiles gearx binary.
    Evidence: .omo/evidence/task-10-binary-compile.txt

  Scenario: Binary name is gearx
    Tool: bash
    Steps:
      1. Run `ls -la target/debug/gearx 2>&1`
    Expected Result: File exists and is executable
    Evidence: .omo/evidence/task-10-binary-exists.txt
  ```

  **Commit**: YES
  - Message: `feat(bin): add gearx desktop binary with engine bootstrap`
  - Files: `src/main.rs`, `Cargo.toml` (if [[bin]] needed)

- [x] 11. **Android platform stub**

  **What to do**:
  - Create `/root/GearX/kernel/src/platform/android.rs` with a minimal Android platform:
    - `AndroidPlatform` struct with `android_app: android_activity::AndroidApp`
    - `impl Platform for AndroidPlatform` — return name "android", stub WindowSystem (returns error), stub TimeSystem (uses std::time::Instant)
    - `impl AndroidWindow` stub — returns minimal Window impl that raises `anyhow::bail!("Android window not implemented in Phase 1")`
    - Entry point function: `#[no_mangle] fn android_main(app: AndroidApp)` that logs "GearX: Android platform stubbed in Phase 1" and returns
  - Gate with `#[cfg(target_os = "android")]` on the module and all structs

  **Must NOT do**:
  - NO actual Android window creation
  - NO NDK cross-compilation plumbing in this task (CI script covers it)
  - NO touch input handling
  - NO SurfaceView/ANativeWindow integration

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: none needed

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 9, 10, 12)
  - **Parallel Group**: Wave 3 (with Tasks 9, 10, 12)
  - **Blocks**: Task 16 (cross-compile CI)
  - **Blocked By**: Task 5

  **References**:
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 135-150 (conditional compilation strategy)
  - android_activity crate: `https://docs.rs/android-activity/latest/android_activity/`

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: Android platform is conditionally compiled
    Tool: ast_grep_search
    Steps:
      1. Pattern: `cfg(target_os = "android")` lang: rust in kernel/src/platform/
    Expected Result: At least 1 cfg gate wrapping Android module or struct
    Evidence: .omo/evidence/task-11-android-cfg.txt

  Scenario: Android stub does not break Linux build
    Tool: bash
    Steps:
      1. Run `cargo build -p gearx_kernel 2>&1`
    Expected Result: Builds successfully on Linux (android module is not compiled)
    Evidence: .omo/evidence/task-11-android-linux-build.txt

  Scenario: Android target cross-compile-checks
    Tool: bash
    Steps:
      1. Run `cargo check -p gearx_kernel --target aarch64-linux-android 2>&1`
    Expected Result: Either success (with NDK installed) or clear linker error messages
    Evidence: .omo/evidence/task-11-android-crosscheck.txt
  ```

  **Commit**: YES (groups with Task 12)
  - Message: `feat(platform): add Android and Web platform stubs`
  - Files: `kernel/src/platform/android.rs`

- [x] 12. **Web platform stub**

  **What to do**:
  - Create `/root/GearX/kernel/src/platform/web.rs` with a minimal Web stub:
    - `WebPlatform` struct
    - `impl Platform for WebPlatform` — return name "web", stub WindowSystem (returns error), stub TimeSystem (uses `web_time::Instant` for WASM compat)
    - `WebWindow` stub — raises `anyhow::bail!("Web window not implemented in Phase 1")`
    - Entry point: `#[wasm_bindgen(start)] pub fn web_main()` that logs "GearX: Web platform stubbed in Phase 1" via `web_sys::console::log_1`
    - Note: wasm32-unknown-unknown has no std::Instant, so use `web_time` crate (re-exports Instant for WASM) or wasm-bindgen's time
  - Gate with `#[cfg(target_arch = "wasm32")]` on the module and all structs

  **Must NOT do**:
  - NO canvas creation
  - NO WebGL/WebGPU surface init
  - NO browser event handling
  - NO JavaScript interop beyond console.log

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: none needed

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 9, 10, 11)
  - **Parallel Group**: Wave 3 (with Tasks 9, 10, 11)
  - **Blocks**: Task 16 (cross-compile CI)
  - **Blocked By**: Task 5

  **References**:
  - Architecture doc: `.omo/plans/gearx-architecture.md` lines 135-150 (conditional compilation strategy)
  - wasm-bindgen docs: `https://rustwasm.github.io/wasm-bindgen/`

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: Web platform uses #[cfg(target_arch = "wasm32")]
    Tool: ast_grep_search
    Steps:
      1. Pattern: `cfg(target_arch = "wasm32")` lang: rust in kernel/src/platform/
    Expected Result: At least 1 cfg gate
    Evidence: .omo/evidence/task-12-web-cfg.txt

  Scenario: Web stub compiles for wasm32 target
    Tool: bash
    Steps:
      1. Run `cargo check -p gearx_kernel --target wasm32-unknown-unknown 2>&1`
    Expected Result: Either success or clear dependency errors
    Evidence: .omo/evidence/task-12-web-crosscheck.txt
  ```

  **Commit**: YES (groups with Task 11)
  - Message: `feat(platform): add Web platform stub with wasm_bindgen entry point`
  - Files: `kernel/src/platform/web.rs`

---

### Wave 4 — QA (ALL PARALLEL)

- [x] 13. **EventBus unit tests**

  **What to do**:
  - Create `/root/GearX/kernel/src/event/tests.rs` with tests:
    - `test_publish_calls_handler` — register a handler that sets a flag, publish event, assert flag was set
    - `test_multiple_handlers` — register 3 handlers for same event, publish, assert all 3 called
    - `test_unrelated_event_not_called` — register handler for EventA, publish EventB, assert handler NOT called
    - `test_handler_removed` — register then remove handler, publish, assert not called
    - `test_publish_no_handler_no_panic` — publish event with zero registered handlers — no crash
    - `test_event_trait_send` — compile-time assertion that Event types are Send
  - Wire `mod tests;` in `kernel/src/event/mod.rs`

  **Must NOT do**:
  - NO async tests
  - NO thread-safety tests (Phase 2 when EventBus is used across threads)

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: none needed

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 14, 15, 16)
  - **Parallel Group**: Wave 4 (with Tasks 14, 15, 16)
  - **Blocks**: Task 15 (integration test uses EventBus)
  - **Blocked By**: Task 6

  **References**:
  - EventBus implementation from Task 6
  - Standard Rust unit test conventions

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: EventBus tests pass
    Tool: bash
    Steps:
      1. Run `cargo test -p gearx_kernel event 2>&1`
    Expected Result: All EventBus tests pass. Output shows "test result: ok. N passed; 0 failed"
    Evidence: .omo/evidence/task-13-eventbus-tests.txt
  ```

  **Commit**: YES (groups with Task 14)
  - Message: `test(kernel): add EventBus and ModuleRegistry unit tests`
  - Files: `kernel/src/event/tests.rs`

- [x] 14. **ModuleRegistry unit tests**

  **What to do**:
  - Create `/root/GearX/kernel/src/module/tests.rs` with tests:
    - `test_register_and_get` — register a mock module, retrieve it by name, assert `Some`
    - `test_register_multiple` — register 2 mock modules, retrieve both by name
    - `test_get_nonexistent` — call get() with unregistered name, assert `None`
    - `test_load_all_calls_init` — register mock that sets init_called flag on init(), call load_all(), assert flag set
    - `test_unload_all_calls_shutdown` — register mock with shutdown flag, call load_all() then unload_all(), assert shutdown called
    - `test_init_error_handling` — register 2 mocks (second fails init), verify first is shutdown on error
    - `test_discover_empty` — call discover() when no #[distributed_slice] modules compiled in, assert empty registry (no panic)
  - Need a mock module: `struct MockModule { name: &'static str, init_ok: bool, init_called: AtomicBool, shutdown_called: AtomicBool }`
  - Wire `mod tests;` in `kernel/src/module/mod.rs`

  **Must NOT do**:
  - NO topological sort tests (deferred)
  - NO dependency resolution tests (deferred)

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: none needed

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 13, 15, 16)
  - **Parallel Group**: Wave 4 (with Tasks 13, 15, 16)
  - **Blocks**: Task 15 (integration test uses ModuleRegistry)
  - **Blocked By**: Task 4

  **References**:
  - ModuleRegistry implementation from Task 4

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: ModuleRegistry tests pass
    Tool: bash
    Steps:
      1. Run `cargo test -p gearx_kernel module 2>&1`
    Expected Result: All ModuleRegistry tests pass. "test result: ok. N passed; 0 failed"
    Evidence: .omo/evidence/task-14-registry-tests.txt
  ```

  **Commit**: YES (groups with Task 13)
  - Message: `test(kernel): add ModuleRegistry unit tests`
  - Files: `kernel/src/module/tests.rs`

- [x] 15. **Integration test — boot→frame→shutdown**

  **What to do**:
  - Create `/root/GearX/tests/boot.rs` (integration test, NOT unit test — separate crate):
    ```rust
    // This test verifies the full Kernel lifecycle:
    // 1. Create Platform (mock or minimal desktop)
    // 2. Create Kernel
    // 3. Run for exactly 3 frames (verify FrameEvent::Start/End are published)
    // 4. Request shutdown
    // 5. Verify clean exit
    //
    // NOTE: This test runs a REAL winit event loop and requires a display.
    // In headless CI, it will be skipped (or fail gracefully).
    ```
  - Key test scenarios:
    - `test_kernel_initializes`: Create Kernel, assert `world` is not empty, assert `modules` has render module, assert `running = false`
    - `test_kernel_boot_and_shutdown`: Create Kernel, call kernel.run() which internally: discovers modules → inits → runs loop → on event → shutdowns. For testing, inject a counter that sends CloseRequested after 3 frames.
  - Location: `/root/GearX/tests/boot.rs` — this is a Cargo integration test (placed in `tests/` dir at workspace root)

  **Must NOT do**:
  - NO headless render test (wgpu needs display)
  - NO GPU-less CI test (will be skipped in CI — marked with `#[ignore]` or `requires_gpu` cfg)

  **Recommended Agent Profile**:
  - **Category**: `unspecified-high`
  - **Skills**: none needed
  - **Reason**: Integration test needs to orchestrate real Kernel lifecycle with winit event loop. Requires careful timeout handling and proper shutdown injection.

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 13, 14, 16)
  - **Parallel Group**: Wave 4 (with Tasks 13, 14, 16)
  - **Blocks**: F1-F4 (verification uses integration test)
  - **Blocked By**: Tasks 10, 13, 14

  **References**:
  - Kernel lifecycle from Task 8
  - Platform implementation from Task 5
  - Cargo integration test docs: `https://doc.rust-lang.org/book/ch11-03-test-organization.html#integration-tests`

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: Integration test compiles
    Tool: bash
    Steps:
      1. Run `cargo test -p gearx --test boot --no-run 2>&1`
    Expected Result: Compiles. "Executable" file generated.
    Evidence: .omo/evidence/task-15-integration-compile.txt

  Scenario: Integration test structure is correct
    Tool: bash
    Steps:
      1. Run `ls tests/boot.rs`
    Expected Result: File exists and is non-empty
    Evidence: .omo/evidence/task-15-integration-exists.txt
  ```

  **Evidence to Capture**:
  - [ ] Integration test source (tests/boot.rs)

  **Commit**: YES
  - Message: `test(kernel): add integration test for Kernel boot, frame, and shutdown`
  - Files: `tests/boot.rs`, `Cargo.toml` (add test deps if needed)

- [x] 16. **Cross-compile CI script + build verification**

  **What to do**:
  - Create `/root/GearX/scripts/verify-phase1.sh`:
    ```bash
    #!/bin/bash
    set -euo pipefail

    echo "=== GearX Phase 1 Verification ==="
    
    echo "--- AC1: Workspace build ---"
    cargo build --workspace 2>&1
    
    echo "--- AC2: Stable Rust (no #![feature]) ---"
    ! grep -r "#!\[feature" kernel/ modules/ src/ 2>/dev/null
    echo "PASS: No nightly features"
    
    echo "--- AC3: Unit tests ---"
    cargo test --workspace 2>&1
    
    echo "--- AC4: Cross-compile checks ---"
    cargo check -p gearx_kernel --target wasm32-unknown-unknown 2>&1 || echo "SKIP: wasm target not installed"
    cargo check -p gearx_kernel --target aarch64-linux-android 2>&1 || echo "SKIP: Android NDK not installed"
    
    echo "=== ALL CHECKS COMPLETE ==="
    ```
  - Make executable: `chmod +x scripts/verify-phase1.sh`
  - Create `/root/GearX/.github/workflows/phase1.yml`:
    - Single job: `ubuntu-latest`
    - Steps: Checkout, Install Rust stable, Install WASM target, `cargo build --workspace`, `cargo test --workspace`, `cargo check --target wasm32-unknown-unknown` (optional — may skip if target not installed)
    - Skip Android cross-compile in CI (requires NDK) — comment with note

  **Must NOT do**:
  - NO full CI pipeline (Phase 2+)
  - NO artifact upload
  - NO continuous deployment

  **Recommended Agent Profile**:
  - **Category**: `quick`
  - **Skills**: none needed

  **Parallelization**:
  - **Can Run In Parallel**: YES (with Tasks 13, 14, 15)
  - **Parallel Group**: Wave 4 (with Tasks 13, 14, 15)
  - **Blocks**: None (verification only)
  - **Blocked By**: Tasks 11, 12

  **References**:
  - GitHub Actions Rust setup: `https://github.com/actions-rs/toolchain`

  **Acceptance Criteria**:

  **QA Scenarios (MANDATORY):**
  ```
  Scenario: Verification script exists and is executable
    Tool: bash
    Steps:
      1. Run `ls -la scripts/verify-phase1.sh`
    Expected Result: File exists with executable permission
    Evidence: .omo/evidence/task-16-script-exists.txt

  Scenario: Verification script runs without crash
    Tool: bash
    Steps:
      1. Run `bash scripts/verify-phase1.sh 2>&1 | head -20`
    Expected Result: Script starts, prints "=== GearX Phase 1 Verification ==="
    Evidence: .omo/evidence/task-16-script-run.txt
  ```

  **Commit**: YES
  - Message: `ci: add Phase 1 build verification script and GitHub Actions workflow`
  - Files: `scripts/verify-phase1.sh`, `.github/workflows/phase1.yml`

---

## Final Verification Wave

> 4 review agents run in PARALLEL. ALL must APPROVE. Present consolidated results to user and get explicit "okay" before completing.

- [x] F1. **Plan Compliance Audit** — `oracle`
  Read the plan end-to-end. For each "Must Have": verify implementation exists (read file, run command). For each "Must NOT Have": search codebase for forbidden patterns — reject with file:line if found. Check evidence files exist in `.omo/evidence/`. Compare deliverables against plan.
  Output: `Must Have [N/N] | Must NOT Have [N/N] | Tasks [N/N] | VERDICT: APPROVE/REJECT`

- [x] F2. **Code Quality Review** — `unspecified-high`
  Run `cargo build --workspace 2>&1 | grep error` for hard errors. Review all changed files for: `as any`/`@ts-ignore` (N/A for Rust), `#[allow(dead_code)]` abuse, `unwrap()` in non-test code, `println!` mixed with tracing, empty catch blocks (`let _ =` swallow), commented-out code, unused imports. Check AI slop: excessive comments, over-abstraction, generic names (data/result/item/temp).
  Output: `Build [PASS/FAIL] | Tests [N pass/N fail] | Files [N clean/N issues] | VERDICT`

- [x] F3. **Real Manual QA** — `unspecified-high` (+ bash)
  Start from clean state (`cargo clean`). Execute EVERY QA scenario from EVERY task — follow exact steps, capture evidence. Test cross-task integration (Kernel boot → modules discovered → window opens → frame renders → close exits clean). Save to `.omo/evidence/final-qa/`.
  Output: `Scenarios [N/N pass] | Integration [N/N] | Edge Cases [N tested] | VERDICT`

- [x] F4. **Scope Fidelity Check** — `deep`
  For each task: read "What to do", read actual diff (git log/diff). Verify 1:1 — everything in spec was built (no missing), nothing beyond spec was built (no creep). Check "Must NOT do" compliance. Detect cross-task contamination: Task N touching Task M's files. Flag unaccounted changes.
  Output: `Tasks [N/N compliant] | Contamination [CLEAN/N issues] | Unaccounted [CLEAN/N files] | VERDICT`

---

## Commit Strategy

- **1**: `chore(workspace): initialize Cargo workspace with root, kernel, render crates`
- **2-3**: `feat(crates): scaffold gearx_kernel and gearx_render crate structure`
- **4-8**: `feat(kernel): implement module system, platform, event bus, logging, and Kernel lifecycle`
- **9**: `feat(render): add RenderModule stub with wgpu init and clear color`
- **10**: `feat(bin): add desktop binary with winit window and engine bootstrap`
- **11-12**: `feat(platform): add Android and Web platform stubs`
- **13-14**: `test(kernel): add EventBus and ModuleRegistry unit tests`
- **15**: `test(kernel): add integration test for boot→frame→shutdown`
- **16**: `ci: add build verification script and cross-compile checks`

---

## Success Criteria

### Verification Commands
```bash
cargo build --workspace 2>&1
# Expected: Compiling gearx_kernel v0.1.0, gearx_render v0.1.0, gearx v0.1.0 — all success

cargo run &
sleep 2 && kill $! 2>/dev/null; wait $! 2>/dev/null
# Expected: stderr contains "GearX Kernel v0.1.0 initialized" and "Module loaded: render"

cargo test --workspace 2>&1
# Expected: test result: OK. N passed; 0 failed

cargo check --target wasm32-unknown-unknown 2>&1
# Expected: Compiling gearx_kernel — success

cargo check --target aarch64-linux-android 2>&1
# Expected: Compiling gearx_kernel — success

grep -r "#!\[feature" kernel/ modules/ src/ 2>/dev/null; echo "Exit: $?"
# Expected: no matches (exit 1 from grep = success)
```

### Final Checklist
- [x] All "Must Have" implemented and verified (F1)
- [x] All "Must NOT Have" absent — no forbidden patterns found (F1)
- [x] All tests pass (F2)
- [x] All QA scenarios pass (F3)
- [x] All tasks within scope — no creep (F4)
- [x] User explicitly approves (after F1-F4 presentation)
