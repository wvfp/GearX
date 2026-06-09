//! Integration tests for the GearX Kernel lifecycle.
//!
//! Tests cover kernel construction, module registration, lifecycle
//! (init → set_mode → shutdown), and the full boot-3-frames lifecycle
//! (marked `#[ignore]` — requires a display).
//!
//! All tests use a mock platform so they can run without a display.
//! The `test_kernel_boot_three_frames_then_shutdown` test uses
//! `DesktopPlatform` and is ignored by default because winit needs X11/Wayland.

use std::time::Duration;

use anyhow::Result;
use gearx_kernel::event::EngineMode;
use gearx_kernel::module::{Module, ModuleRegistry, ModuleState};
use gearx_kernel::platform::{Platform, PlatformEvent, TimeSystem, Window, WindowSystem};
use gearx_kernel::Kernel;

// ---------------------------------------------------------------------------
// Mock platform — all methods that create windows or run event loops panic
// because our tests never exercise them.
// ---------------------------------------------------------------------------

struct MockWindowSystem;

impl WindowSystem for MockWindowSystem {
    fn create_window(
        &mut self,
        _title: &str,
        _width: u32,
        _height: u32,
    ) -> Box<dyn Window> {
        panic!("create_window not supported in mock — tests never create windows");
    }

    fn run_event_loop(&mut self, _on_event: &mut dyn FnMut(PlatformEvent)) {
        panic!("run_event_loop not supported in mock — tests never run the event loop");
    }
}

struct MockTimeSystem;

impl TimeSystem for MockTimeSystem {
    fn now(&self) -> Duration {
        Duration::ZERO
    }

    fn delta_seconds(&mut self) -> f32 {
        0.016 // ~60 FPS
    }
}

struct MockPlatform {
    window_system: MockWindowSystem,
    time: MockTimeSystem,
}

impl MockPlatform {
    fn new() -> Self {
        Self {
            window_system: MockWindowSystem,
            time: MockTimeSystem,
        }
    }
}

impl Platform for MockPlatform {
    fn name(&self) -> &'static str {
        "mock-test"
    }

    fn window_system(&mut self) -> &mut dyn WindowSystem {
        &mut self.window_system
    }

    fn time_system(&mut self) -> &mut dyn TimeSystem {
        &mut self.time
    }
}

// ---------------------------------------------------------------------------
// A minimal module implementation for testing discovery / lifecycle
// ---------------------------------------------------------------------------

struct TestModule {
    name: &'static str,
}

impl Module for TestModule {
    fn name(&self) -> &'static str {
        self.name
    }

    fn init(&mut self, _kernel: &mut Kernel) -> Result<()> {
        Ok(())
    }

    fn update(&mut self, _kernel: &mut Kernel, _dt: f32) -> Result<()> {
        Ok(())
    }

    fn shutdown(&mut self, _kernel: &mut Kernel) -> Result<()> {
        Ok(())
    }
}

// ===========================================================================
// Tests
// ===========================================================================

/// Construct a kernel with the mock platform and verify its initial state.
#[test]
fn test_kernel_construct() -> Result<()> {
    let mut kernel = Kernel::new(Box::new(MockPlatform::new()));

    assert!(kernel.is_running(), "fresh kernel should be running");
    assert_eq!(
        kernel.mode(),
        EngineMode::Edit,
        "fresh kernel should start in Edit mode"
    );
    assert_eq!(
        kernel.registry().len(),
        0,
        "fresh kernel should have no modules registered"
    );

    Ok(())
}

/// Verify the module registry can store modules and track their states.
#[test]
fn test_kernel_discover_modules() -> Result<()> {
    let mut reg = ModuleRegistry::new();

    reg.register(Box::new(TestModule { name: "alpha" }));
    reg.register(Box::new(TestModule { name: "beta" }));

    assert_eq!(reg.len(), 2, "two modules should be registered");
    assert_eq!(
        reg.state("alpha"),
        Some(ModuleState::Registered),
        "alpha should be in Registered state"
    );
    assert_eq!(
        reg.state("beta"),
        Some(ModuleState::Registered),
        "beta should be in Registered state"
    );
    assert_eq!(
        reg.state("gamma"),
        None,
        "gamma was not registered"
    );

    Ok(())
}

/// Full kernel lifecycle: construct → set_mode → shutdown → drop.
#[test]
fn test_kernel_initializes() -> Result<()> {
    let mut kernel = Kernel::new(Box::new(MockPlatform::new()));

    // ── Initial state ──
    assert!(kernel.is_running(), "kernel should be running after construction");
    assert_eq!(kernel.mode(), EngineMode::Edit, "kernel should start in Edit mode");

    // ── Mode transition ──
    kernel.set_mode(EngineMode::Play);
    assert_eq!(kernel.mode(), EngineMode::Play, "mode should be Play after set_mode(Play)");

    kernel.set_mode(EngineMode::Pause);
    assert_eq!(kernel.mode(), EngineMode::Pause, "mode should be Pause after set_mode(Pause)");

    // ── Shutdown ──
    kernel.shutdown();
    assert!(!kernel.is_running(), "kernel should not be running after shutdown");

    // Idempotent shutdown
    kernel.shutdown();
    assert!(!kernel.is_running(), "kernel should stay stopped on second shutdown");

    // ── Drop ──
    // Kernel is dropped here; the test passes if drop doesn't panic.
    Ok(())
}

/// Full boot-3-frames-then-shutdown lifecycle using the real desktop platform.
///
/// This test requires a display (X11/Wayland) and blocks on winit's event loop,
/// so it is ignored by default.  Run it explicitly with:
///
/// ```ignore
/// cargo test -p gearx --test boot test_kernel_boot_three_frames -- --ignored
/// ```
#[test]
#[ignore = "requires a display (winit needs X11 or Wayland)"]
fn test_kernel_boot_three_frames_then_shutdown() -> Result<()> {
    // DesktopPlatform::new() opens an X11/Wayland connection.
    let platform = gearx_kernel::platform::desktop::DesktopPlatform::new()?;
    let mut kernel = Kernel::new(Box::new(platform));

    assert!(kernel.is_running(), "kernel should be running after construction with DesktopPlatform");
    assert_eq!(kernel.mode(), EngineMode::Edit);

    // Note: A full 3-frame lifecycle would require integrating with winit's
    // event loop to pump events and emit CloseRequested after 3 ticks.
    // For now, this test verifies that DesktopPlatform + Kernel construction
    // works correctly.
    //
    // Future work: spawn the kernel.run() in a separate thread and signal
    // shutdown by posting a CloseRequested event to the winit loop.

    kernel.shutdown();
    assert!(!kernel.is_running(), "kernel should stop after explicit shutdown");

    Ok(())
}
