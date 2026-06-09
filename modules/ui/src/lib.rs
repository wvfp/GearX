use std::sync::{Arc, Mutex};

use anyhow::Result;
use gearx_kernel::event::EventHandler;
use gearx_kernel::module::Module;
use gearx_kernel::platform::PlatformEvent;
use gearx_kernel::Kernel;
use gearx_render::{EguiRenderData, RenderContext};
use linkme::distributed_slice;

mod panels;

type PendingEvents = Arc<Mutex<Vec<PlatformEvent>>>;

pub struct UiModule {
    egui_ctx: egui::Context,
    pending: PendingEvents,
    modifiers: egui::Modifiers,
    scale_factor: f32,
    screen_size: [f32; 2],
}

impl UiModule {
    pub fn new(render_ctx: &RenderContext) -> Self {
        let screen_size = {
            let s = render_ctx.screen_size.lock().expect("screen size lock");
            [s.0 as f32, s.1 as f32]
        };
        Self {
            egui_ctx: egui::Context::default(),
            pending: Arc::new(Mutex::new(Vec::new())),
            modifiers: egui::Modifiers::default(),
            scale_factor: 1.0,
            screen_size,
        }
    }

    fn process_events(&mut self) -> egui::RawInput {
        let events: Vec<PlatformEvent> = {
            let mut q = self.pending.lock().expect("pending events lock");
            q.drain(..).collect()
        };
        let mut raw = egui::RawInput {
            screen_rect: Some(egui::Rect::from_min_size(
                egui::Pos2::ZERO,
                egui::Vec2::new(self.screen_size[0], self.screen_size[1]),
            )),
            ..Default::default()
        };
        for event in events {
            match event {
                PlatformEvent::CursorMoved(x, y) => {
                    raw.events.push(egui::Event::PointerMoved(egui::pos2(x, y)));
                }
                PlatformEvent::MouseButtonPressed(btn) => {
                    if let Some(button) = mouse_button(btn) {
                        raw.events.push(egui::Event::PointerButton { pos: egui::Pos2::ZERO, button, pressed: true, modifiers: self.modifiers });
                    }
                }
                PlatformEvent::MouseButtonReleased(btn) => {
                    if let Some(button) = mouse_button(btn) {
                        raw.events.push(egui::Event::PointerButton { pos: egui::Pos2::ZERO, button, pressed: false, modifiers: self.modifiers });
                    }
                }
                PlatformEvent::MouseWheel(dx, dy) => {
                    raw.events.push(egui::Event::MouseWheel { unit: egui::MouseWheelUnit::Point, delta: egui::vec2(dx, dy), modifiers: self.modifiers });
                }
                PlatformEvent::TextInput(text) => { raw.events.push(egui::Event::Text(text)); }
                PlatformEvent::KeyPressed(sc) => {
                    if let Some(key) = scancode_to_egui(sc) {
                        raw.events.push(egui::Event::Key { key, physical_key: None, pressed: true, repeat: false, modifiers: self.modifiers });
                    }
                }
                PlatformEvent::KeyReleased(sc) => {
                    if let Some(key) = scancode_to_egui(sc) {
                        raw.events.push(egui::Event::Key { key, physical_key: None, pressed: false, repeat: false, modifiers: self.modifiers });
                    }
                }
                PlatformEvent::ModifiersChanged { shift, ctrl, alt, logo } => {
                    self.modifiers = egui::Modifiers { shift, ctrl, alt, mac_cmd: logo, command: ctrl };
                    raw.modifiers = self.modifiers;
                }
                PlatformEvent::ScaleFactorChanged(f) => { self.scale_factor = f as f32; }
                PlatformEvent::Resized(w, h) => { self.screen_size = [w as f32, h as f32]; }
                _ => {}
            }
        }
        raw.modifiers = self.modifiers;
        raw
    }
}

fn mouse_button(b: u8) -> Option<egui::PointerButton> {
    match b { 0 => Some(egui::PointerButton::Primary), 1 => Some(egui::PointerButton::Secondary), 2 => Some(egui::PointerButton::Middle), _ => None }
}

fn scancode_to_egui(sc: u32) -> Option<egui::Key> {
    match sc {
        0x0004 => Some(egui::Key::A), 0x0007 => Some(egui::Key::D),
        0x0016 => Some(egui::Key::S), 0x001A => Some(egui::Key::W),
        0x0028 => Some(egui::Key::Enter), 0x0029 => Some(egui::Key::Escape),
        0x002C => Some(egui::Key::Space),
        0x0054 => Some(egui::Key::ArrowDown), 0x0055 => Some(egui::Key::ArrowLeft),
        0x0056 => Some(egui::Key::ArrowRight), 0x0057 => Some(egui::Key::ArrowUp),
        _ => None,
    }
}

struct PlatformEventBridge { pending: PendingEvents }

impl EventHandler<PlatformEvent> for PlatformEventBridge {
    fn handle(&mut self, event: &PlatformEvent) {
        if let Ok(mut q) = self.pending.lock() { q.push(event.clone()); }
    }
}

impl Module for UiModule {
    fn name(&self) -> &'static str { "ui" }

    fn init(&mut self, kernel: &mut Kernel) -> Result<()> {
        let bridge = PlatformEventBridge { pending: Arc::clone(&self.pending) };
        kernel.event_bus().listen::<PlatformEvent, _>(bridge);
        Ok(())
    }

    fn update(&mut self, kernel: &mut Kernel, _dt: f32) -> Result<()> {
        if let Some(ctx) = kernel.world().get_resource::<RenderContext>() {
            if let Ok(size) = ctx.screen_size.lock() {
                self.screen_size = [size.0 as f32, size.1 as f32];
            }
        }

        let raw_input = self.process_events();

        let fps = kernel.world().get_resource::<gearx_kernel::FrameStats>().map(|s| s.fps).unwrap_or(0.0);
        let frame_count = kernel.world().get_resource::<gearx_kernel::Time>().map(|t| t.frame_count).unwrap_or(0);
        let mode = kernel.mode();
        let profiler_report = kernel.profiler().report();

        let full_output = self.egui_ctx.run(raw_input, |ctx| {
            panels::debug_panels(ctx, fps, frame_count, mode, &profiler_report);
        });

        let clipped = self.egui_ctx.tessellate(full_output.shapes, full_output.pixels_per_point);

        // Write tessellated data to EguiRenderData for the RenderModule to consume.
        if let Some(mut data) = kernel.world().get_resource_mut::<EguiRenderData>() {
            data.primitives = clipped;
            data.screen_descriptor.size_in_pixels = [self.screen_size[0] as u32, self.screen_size[1] as u32];
            data.screen_descriptor.pixels_per_point = self.scale_factor;
        }

        Ok(())
    }

    fn shutdown(&mut self, _kernel: &mut Kernel) -> Result<()> { Ok(()) }
}

#[distributed_slice(gearx_kernel::module::MODULES)]
pub static REGISTER_UI: fn() -> Box<dyn Module> = || -> Box<dyn Module> {
    panic!("UiModule requires RenderContext — register manually");
};
