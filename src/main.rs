use anyhow::Result;
use gearx_kernel::ecs::{FrameStats, Time};
use gearx_kernel::init::init_logging;
use gearx_kernel::platform::Platform;
use gearx_kernel::scene::loader::{ComponentRegistry, SceneLoader};
use gearx_kernel::{Res, ResMut, Kernel};
use gearx_input::{InputState, KeyCode};
use gearx_render::{RenderModule, RenderOffset, RenderContext};
use gearx_ui::UiModule;

fn log_stats_system(time: Res<Time>, stats: Res<FrameStats>, mut frame_count: ResMut<Time>) {
    frame_count.frame_count = time.frame_count;
    if time.frame_count % 60 == 0 {
        tracing::info!(
            "Demo — frame {} | FPS {:.0} | elapsed {:.1}s",
            time.frame_count, stats.fps, time.elapsed,
        );
    }
}

fn movement_system(
    input: Res<InputState>,
    time: Res<Time>,
    mut offset: ResMut<RenderOffset>,
) {
    let speed = 1.5;
    let dt = time.delta;
    if input.keys_pressed[KeyCode::ArrowLeft as usize] { offset.x -= speed * dt; }
    if input.keys_pressed[KeyCode::ArrowRight as usize] { offset.x += speed * dt; }
    if input.keys_pressed[KeyCode::ArrowUp as usize] { offset.y += speed * dt; }
    if input.keys_pressed[KeyCode::ArrowDown as usize] { offset.y -= speed * dt; }
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

    // Create and register UI module (needs RenderContext from render module init).
    if let Some(ctx) = kernel.world().get_resource::<RenderContext>() {
        let ui_module = UiModule::new(ctx);
        kernel.registry().register(Box::new(ui_module));
        tracing::info!("UI module initialized");
    }

    // Load demo scene.
    kernel.world().insert_resource(ComponentRegistry::new());
    match SceneLoader::load(std::path::Path::new("assets/scenes/demo.scene"), kernel.world()) {
        Ok(entities) => tracing::info!("Loaded demo scene: {} entities", entities.len()),
        Err(e) => tracing::warn!("Failed to load demo scene: {e:#}"),
    }

    kernel.add_system(log_stats_system);
    kernel.add_system(movement_system);

    tracing::info!(
        "Kernel initialized — {} module(s), {} ECS system(s)",
        kernel.registry().len(), 2,
    );

    kernel.run();
    tracing::info!("Engine shut down");
    Ok(())
}
