// GearX Engine Binary Entry Point
//
// Initialises the desktop platform, creates a window, builds the render
// module, constructs the kernel, and runs the engine main loop.

use anyhow::Result;
use gearx_kernel::ecs::{FrameStats, Time};
use gearx_kernel::init::init_logging;
use gearx_kernel::platform::Platform;
use gearx_kernel::{Res, ResMut, Kernel};
use gearx_render::RenderModule;

/// System that logs frame timing every ~60 frames (roughly once per second).
fn log_stats_system(time: Res<Time>, stats: Res<FrameStats>, mut frame_count: ResMut<Time>) {
    // Update the `Time` resource's frame count for the demo.
    frame_count.frame_count = time.frame_count;
    if time.frame_count % 60 == 0 {
        tracing::info!(
            "Demo — frame {} | FPS {:.0} | elapsed {:.1}s",
            time.frame_count,
            stats.fps,
            time.elapsed,
        );
    }
}

fn main() -> Result<()> {
    init_logging();
    tracing::info!("GearX Engine starting");

    // Create platform and window
    let mut platform = gearx_kernel::platform::desktop::DesktopPlatform::new()?;
    let window = Box::leak(platform.window_system().create_window("GearX", 1280, 720));
    tracing::info!("Window created: {}x{}", window.size().0, window.size().1);

    // Create render module (async wgpu init, blocked on main thread)
    let render_module = pollster::block_on(RenderModule::new(window))?;
    tracing::info!("Render module initialized");

    // Create kernel and register render module.
    // Input and asset modules are auto-discovered via linkme (their factories
    // don't panic — only the render factory panics, which is caught).
    let mut kernel = Kernel::new(Box::new(platform));
    kernel.registry().register(Box::new(render_module));

    // Register demo ECS systems.
    kernel.add_system(log_stats_system);

    tracing::info!(
        "Kernel initialized — {} module(s), {} ECS system(s)",
        kernel.registry().len(),
        1, // only log_stats_system for Phase 2
    );

    // Run the engine (blocks until shutdown).
    kernel.run();
    tracing::info!("Engine shut down");

    Ok(())
}