use gearx_kernel::event::EngineMode;

pub fn debug_panels(
    ctx: &egui::Context,
    fps: f32,
    frame_count: u64,
    mode: EngineMode,
    profiler_report: &str,
) {
    // FPS overlay — always visible.
    egui::Window::new("FPS")
        .default_pos([10.0, 10.0])
        .resizable(false)
        .title_bar(false)
        .show(ctx, |ui| {
            ui.label(format!("{:.0} FPS | Frame {}", fps, frame_count));
        });

    // Detailed panels — only in Edit mode.
    if mode == EngineMode::Edit {
        egui::Window::new("Debug")
            .default_pos([10.0, 50.0])
            .default_width(300.0)
            .show(ctx, |ui| {
                ui.label(format!("Mode: {:?}", mode));
                ui.separator();
                ui.label("Profiler:");
                ui.label(profiler_report);
            });
    }
}
