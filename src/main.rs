use anyhow::Result;
use gearx_kernel::ecs::{FrameStats, Time};
use gearx_kernel::init::init_logging;
use gearx_kernel::platform::Platform;
use gearx_kernel::{Res, ResMut, Kernel};
use gearx_input::{InputState, KeyCode};
use gearx_render::{RenderModule, RenderOffset};

/// System that logs frame timing every ~60 frames (roughly once per second).
fn log_stats_system(time: Res<Time>, stats: Res<FrameStats>, mut frame_count: ResMut<Time>) {
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

/// System that moves the triangle based on keyboard input.
fn movement_system(
    input: Res<InputState>,
    time: Res<Time>,
    mut offset: ResMut<RenderOffset>,
) {
    let speed = 1.5; // NDC units per second
    let dt = time.delta;
    if input.keys_pressed[KeyCode::ArrowLeft as usize] {
        offset.x -= speed * dt;
    }
    if input.keys_pressed[KeyCode::ArrowRight as usize] {
        offset.x += speed * dt;
    }
    if input.keys_pressed[KeyCode::ArrowUp as usize] {
        offset.y += speed * dt;
    }
    if input.keys_pressed[KeyCode::ArrowDown as usize] {
        offset.y -= speed * dt;
    }
}

fn main() -> Result<()> {
    init_logging();
    tracing::info!("GearX Engine starting");

    let mut platform = gearx_kernel::platform::desktop::DesktopPlatform::new()?;
    let window = Box::leak(platform.window_system().create_window("GearX", 1280, 720));
    tracing::info!("Window created: {}x{}", window.size().0, window.size().1);

    let render_module = pollster::block_on(RenderModule::new(window))?;
    tracing::info!("Render module initialized");

    let mut kernel = Kernel::new(Box::new(platform));
    kernel.registry().register(Box::new(render_module));

    kernel.add_system(log_stats_system);
    kernel.add_system(movement_system);

    tracing::info!(
        "Kernel initialized — {} module(s), {} ECS system(s)",
        kernel.registry().len(),
        2,
    );

    kernel.run();
    tracing::info!("Engine shut down");

    Ok(())
}
