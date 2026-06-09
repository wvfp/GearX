//! Web (WASM) platform stub.
//!
//! This module provides a minimal [`Platform`] implementation for WebAssembly.
//! It is compiled only when `target_arch = "wasm32"`.

use std::time::Duration;

use crate::platform::{Platform, PlatformEvent, TimeSystem, Window, WindowSystem};

// ---------------------------------------------------------------------------
// Time
// ---------------------------------------------------------------------------

pub struct WebTime;

impl TimeSystem for WebTime {
    fn now(&self) -> Duration {
        Duration::default()
    }

    fn delta_seconds(&mut self) -> f32 {
        0.0
    }
}

// ---------------------------------------------------------------------------
// Window stub
// ---------------------------------------------------------------------------

pub struct WebWindow;

impl Window for WebWindow {
    fn raw_window_handle(&self) -> raw_window_handle::WindowHandle<'_> {
        panic!("WebWindow not yet implemented")
    }

    fn size(&self) -> (u32, u32) {
        (0, 0)
    }
}

// ---------------------------------------------------------------------------
// Window system stub
// ---------------------------------------------------------------------------

pub struct WebWindowSystem;

impl WindowSystem for WebWindowSystem {
    fn create_window(&mut self, _title: &str, _width: u32, _height: u32) -> Box<dyn Window> {
        Box::new(WebWindow)
    }

    fn run_event_loop(&mut self, _on_event: &mut dyn FnMut(PlatformEvent)) {
        // Stub — no-op until Web backend is fully implemented.
    }
}

// ---------------------------------------------------------------------------
// Platform
// ---------------------------------------------------------------------------

pub struct WebPlatform {
    window_system: WebWindowSystem,
    time: WebTime,
}

impl WebPlatform {
    pub fn new() -> Self {
        Self {
            window_system: WebWindowSystem,
            time: WebTime,
        }
    }
}

impl Platform for WebPlatform {
    fn name(&self) -> &'static str {
        "web"
    }

    fn window_system(&mut self) -> &mut dyn WindowSystem {
        &mut self.window_system
    }

    fn time_system(&mut self) -> &mut dyn TimeSystem {
        &mut self.time
    }
}
