// GearX Engine Binary Entry Point
//
// Initialises the desktop platform, creates a window, builds the render
// module, constructs the kernel, and runs the engine main loop.

use anyhow::Result;
use gearx_kernel::init::init_logging;
use gearx_kernel::platform::Platform;
use gearx_kernel::Kernel;
use gearx_render::RenderModule;

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

    // Create kernel and register render module
    let mut kernel = Kernel::new(Box::new(platform));
    kernel.registry().register(Box::new(render_module));
    tracing::info!(
        "Kernel initialized with {} module(s)",
        kernel.registry().len()
    );

    // Run the engine (blocks until shutdown)
    kernel.run();
    tracing::info!("Engine shut down");

    Ok(())
}
