//! Android platform stub.
//!
//! This module provides a minimal [`Platform`] implementation for Android.
//! It is compiled only when `target_os = "android"`.

use std::time::Duration;

use crate::platform::{Platform, PlatformEvent, TimeSystem, Window, WindowSystem};

// ---------------------------------------------------------------------------
// Time
// ---------------------------------------------------------------------------

pub struct AndroidTime;

impl TimeSystem for AndroidTime {
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

pub struct AndroidWindow;

impl Window for AndroidWindow {
    fn raw_window_handle(&self) -> raw_window_handle::WindowHandle<'_> {
        panic!("AndroidWindow not yet implemented")
    }

    fn size(&self) -> (u32, u32) {
        (0, 0)
    }
}

// ---------------------------------------------------------------------------
// Window system stub
// ---------------------------------------------------------------------------

pub struct AndroidWindowSystem;

impl WindowSystem for AndroidWindowSystem {
    fn create_window(&mut self, _title: &str, _width: u32, _height: u32) -> Box<dyn Window> {
        Box::new(AndroidWindow)
    }

    fn run_event_loop(&mut self, _on_event: &mut dyn FnMut(PlatformEvent)) {
        // Stub — no-op until Android backend is fully implemented.
    }
}

// ---------------------------------------------------------------------------
// Platform
// ---------------------------------------------------------------------------

pub struct AndroidPlatform {
    window_system: AndroidWindowSystem,
    time: AndroidTime,
}

impl AndroidPlatform {
    pub fn new() -> Self {
        Self {
            window_system: AndroidWindowSystem,
            time: AndroidTime,
        }
    }
}

impl Platform for AndroidPlatform {
    fn name(&self) -> &'static str {
        "android"
    }

    fn window_system(&mut self) -> &mut dyn WindowSystem {
        &mut self.window_system
    }

    fn time_system(&mut self) -> &mut dyn TimeSystem {
        &mut self.time
    }
}
