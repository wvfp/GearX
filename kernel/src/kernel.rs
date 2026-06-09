//! Real `Kernel` struct with lifecycle management.
//!
//! Owns the module registry, platform abstraction, event bus, frame counter,
//! and engine mode state.  Provides the main loop (`run()`) and lifecycle
//! methods (`shutdown`, `set_mode`).
//!
//! # Borrow-checker workaround
//!
//! `ModuleRegistry` methods (`load_all`, `iter_mut`, `unload_all`) take
//! `&mut Kernel` so that modules can access the kernel during init/update/
//! shutdown.  When `Kernel` itself owns the registry, this creates a
//! self-borrow: we cannot mutably borrow both `self.registry` and `self`
//! simultaneously.
//!
//! The workaround is `std::mem::take` — we temporarily swap the registry
//! with an empty default, operate on the taken value (passing `&mut self`
//! to module callbacks), and then swap it back.  This is sound because
//! `Box<dyn Module>` entries are heap-allocated and don't alias with other
//! kernel fields.
//!
//! In Phase 1 this overhead is negligible; a future refactor may introduce
//! a `KernelContext` split to eliminate it.

use crate::event::{EngineEvent, EngineMode, EventBus, FrameEvent};
use crate::frame_counter::FrameCounter;
use crate::module::ModuleRegistry;
use crate::platform::Platform;
use crate::task::TaskSystem;
use bevy_ecs::prelude::{IntoSystem, Resource, Schedule, World};

/// The central engine kernel — owns all engine subsystems.
pub struct Kernel {
    /// ECS world — stores all entities, components, and resources.
    pub world: World,
    /// ECS schedule — defines and runs systems each frame.
    pub schedule: Schedule,
    /// Parallel task dispatcher.
    task_system: TaskSystem,
    /// Registry of all loaded engine modules.
    registry: ModuleRegistry,
    /// Platform abstraction (windowing, timing, etc.).
    platform: Box<dyn Platform>,
    /// Synchronous in-process event bus.
    event_bus: EventBus,
    /// Frame-rate counter and FPS reporter.
    frame_counter: FrameCounter,
    /// Current engine mode (Edit / Play / Pause).
    mode: EngineMode,
    /// Whether the main loop should keep running.
    running: bool,
}

impl Kernel {
    /// Create a new kernel with the given platform backend.
    ///
    /// The caller is responsible for initialising logging *before* calling
    /// this constructor (e.g. via [`crate::init::init_logging`]).
    pub fn new(platform: Box<dyn Platform>) -> Self {
        let mut world = World::new();
        world.insert_resource(crate::ecs::Time::default());
        world.insert_resource(crate::ecs::FrameStats::default());

        Self {
            world,
            schedule: Schedule::default(),
            task_system: TaskSystem::default(),
            registry: ModuleRegistry::new(),
            platform,
            event_bus: EventBus::new(),
            frame_counter: FrameCounter::new(),
            mode: EngineMode::Edit,
            running: true,
        }
    }

    /// Run the engine main loop.
    ///
    /// In Phase 1 this is a simple frame-sleep loop (~60 FPS) that:
    ///
    /// 1. Discovers and loads all linked modules.
    /// 2. Publishes `FrameEvent::Start` at the beginning of each frame.
    /// 3. Calls `update()` on every active module.
    /// 4. Publishes `FrameEvent::End` at the end of each frame.
    /// 5. Sleeps for ~16 ms to cap the frame rate.
    ///
    /// The platform event loop is **not** integrated yet — the binary is
    /// responsible for creating windows and running the platform loop
    /// separately.
    pub fn run(&mut self) {
        tracing::info!("GearX Kernel v{} starting", crate::init::KERNEL_VERSION);

        // --- Load modules ---
        // First discover auto-registered modules (input, assets, etc.).
        // Panicking factories (e.g. RenderModule) are caught and skipped —
        // those are registered manually by the binary.
        // Temporarily detach the registry to avoid a self-borrow when
        // passing `&mut self` to `load_all`.  `ModuleRegistry` implements
        // `Default` (empty registry), so `take` is cheap.
        {
            let mut reg = std::mem::take(&mut self.registry);
            reg.discover();
            if let Err(e) = reg.load_all(self) {
                tracing::error!("Failed to load modules: {e:#}");
                self.registry = reg;
                return;
            }
            self.registry = reg;
        }

        // --- Main loop ---
        while self.running {
            // Poll platform events (window close, resize, etc.) non-blocking.
            let platform_events = self.platform.window_system().poll_events();
            if platform_events
                .iter()
                .any(|e| matches!(e, crate::platform::PlatformEvent::CloseRequested))
            {
                break;
            }

            let dt = self.platform.time_system().delta_seconds();
            self.event_bus.publish(&FrameEvent::Start(dt));

            // Mutably iterate modules while still allowing them to access the
            // kernel — same take pattern as above.
            {
                let mut reg = std::mem::take(&mut self.registry);
                for module in reg.iter_mut() {
                    if let Err(e) = module.update(self, dt) {
                        tracing::error!("Module '{}' update error: {e:#}", module.name());
                    }
                }
                self.registry = reg;
            }

            // Run the ECS schedule (executes all registered systems).
            self.task_system.run_schedule(&mut self.world, &mut self.schedule);

            self.event_bus.publish(&FrameEvent::End(dt));
            self.frame_counter.tick();

            // Cap frame rate at ~60 FPS for Phase 1.
            std::thread::sleep(std::time::Duration::from_millis(16));
        }

        self.shutdown();
    }

    /// Gracefully shut down the engine.
    ///
    /// Publishes the `Shutdown` event, unloads all modules in reverse
    /// registration order, and marks the kernel as stopped.
    pub fn shutdown(&mut self) {
        if !self.running {
            return; // already shut down
        }

        self.event_bus.publish(&EngineEvent::Shutdown);

        {
            let mut reg = std::mem::take(&mut self.registry);
            if let Err(e) = reg.unload_all(self) {
                tracing::warn!("Module unload error during shutdown: {e:#}");
            }
            self.registry = reg;
        }

        self.running = false;
        tracing::info!("GearX Kernel shut down");
    }

    /// Change the engine mode (Edit / Play / Pause) and broadcast the change.
    pub fn set_mode(&mut self, mode: EngineMode) {
        self.mode = mode;
        self.event_bus.publish(&EngineEvent::ModeChanged(mode));
    }

    // ── Accessors ──

    /// Mutable reference to the module registry.
    pub fn registry(&mut self) -> &mut ModuleRegistry {
        &mut self.registry
    }

    /// Mutable reference to the platform abstraction.
    pub fn platform(&mut self) -> &mut dyn Platform {
        &mut *self.platform
    }

    /// Mutable reference to the event bus.
    pub fn event_bus(&mut self) -> &mut EventBus {
        &mut self.event_bus
    }

    /// Mutable reference to the frame counter.
    pub fn frame_counter(&mut self) -> &mut FrameCounter {
        &mut self.frame_counter
    }

    /// Current engine mode.
    #[must_use]
    pub fn mode(&self) -> EngineMode {
        self.mode
    }

    /// Whether the kernel is still running (i.e. the main loop should continue).
    #[must_use]
    pub fn is_running(&self) -> bool {
        self.running
    }

    // ── ECS integration ──

    /// Register a system to run every frame via the ECS schedule.
    ///
    /// The system can access resources and components through bevy_ecs query
    /// parameters (`Res`, `ResMut`, `Query`, etc.).
    pub fn add_system<M>(&mut self, system: impl IntoSystem<(), (), M> + 'static) {
        self.schedule.add_systems(system);
    }

    /// Insert a resource into the ECS world.
    ///
    /// Resources are globally-accessible singletons.  If a resource of the
    /// same type already exists it will be replaced.
    pub fn add_resource<T: Resource>(&mut self, resource: T) {
        self.world.insert_resource(resource);
    }

    /// Mutable reference to the ECS world.
    pub fn world(&mut self) -> &mut World {
        &mut self.world
    }
}

// ── Test helpers ──

#[cfg(test)]
pub(crate) mod test_helpers {
    use std::time::Duration;

    use super::*;
    use crate::platform::{TimeSystem, Window, WindowSystem};

    /// A window system that panics if called — the test platform never
    /// exercises window creation or event loops.
    pub(crate) struct NullWindowSystem;

    impl WindowSystem for NullWindowSystem {
        fn create_window(
            &mut self,
            _title: &str,
            _width: u32,
            _height: u32,
        ) -> Box<dyn Window> {
            panic!("NullWindowSystem should not be called in tests");
        }

        fn run_event_loop(
            &mut self,
            _on_event: &mut dyn FnMut(crate::platform::PlatformEvent),
        ) {
            panic!("NullWindowSystem should not be called in tests");
        }
    }

    /// A time system that always returns a fixed ~60 FPS delta.
    pub(crate) struct NullTime;

    impl TimeSystem for NullTime {
        fn now(&self) -> Duration {
            Duration::ZERO
        }

        fn delta_seconds(&mut self) -> f32 {
            0.016 // ~60 FPS
        }
    }

    /// A minimal platform implementation for testing.
    pub(crate) struct NullPlatform {
        time: NullTime,
        window_system: NullWindowSystem,
    }

    impl Platform for NullPlatform {
        fn name(&self) -> &'static str {
            "null-test"
        }

        fn window_system(&mut self) -> &mut dyn WindowSystem {
            &mut self.window_system
        }

        fn time_system(&mut self) -> &mut dyn TimeSystem {
            &mut self.time
        }
    }

    impl Kernel {
        /// Create a `Kernel` instance backed by a null platform, suitable
        /// for unit tests that don't exercise platform functionality.
        pub fn test() -> Self {
            Self::new(Box::new(NullPlatform {
                time: NullTime,
                window_system: NullWindowSystem,
            }))
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::event::EngineMode;

    #[test]
    fn new_kernel_has_edit_mode() {
        let kernel = Kernel::test();
        assert_eq!(kernel.mode(), EngineMode::Edit);
    }

    #[test]
    fn new_kernel_is_running() {
        let kernel = Kernel::test();
        assert!(kernel.is_running());
    }

    #[test]
    fn shutdown_stops_kernel() {
        let mut kernel = Kernel::test();
        kernel.shutdown();
        assert!(!kernel.is_running());
    }

    #[test]
    fn double_shutdown_is_idempotent() {
        let mut kernel = Kernel::test();
        kernel.shutdown();
        kernel.shutdown(); // second call should be a no-op
        assert!(!kernel.is_running());
    }

    #[test]
    fn set_mode_changes_mode() {
        let mut kernel = Kernel::test();
        assert_eq!(kernel.mode(), EngineMode::Edit);

        kernel.set_mode(EngineMode::Play);
        assert_eq!(kernel.mode(), EngineMode::Play);

        kernel.set_mode(EngineMode::Pause);
        assert_eq!(kernel.mode(), EngineMode::Pause);

        // Back to edit
        kernel.set_mode(EngineMode::Edit);
        assert_eq!(kernel.mode(), EngineMode::Edit);
    }

    #[test]
    fn set_mode_does_not_panic() {
        let mut kernel = Kernel::test();
        // Smoke test: set_mode should not panic regardless of handler registration.
        kernel.set_mode(EngineMode::Play);
        kernel.set_mode(EngineMode::Pause);
        kernel.set_mode(EngineMode::Edit);
    }

    #[test]
    fn accessors_are_available() {
        let mut kernel = Kernel::test();

        // All accessors should return something (not panic)
        let _ = kernel.registry();
        let _ = kernel.platform();
        let _ = kernel.event_bus();
        let _ = kernel.frame_counter();
    }
}
