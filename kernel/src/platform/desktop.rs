use std::time::{Duration, Instant};

use raw_window_handle::{HasDisplayHandle, HasWindowHandle, WindowHandle};
use winit::dpi::LogicalSize;
use winit::event::{Event, WindowEvent};
use winit::event_loop::{ActiveEventLoop, EventLoop};
use winit::platform::pump_events::EventLoopExtPumpEvents;
use winit::platform::run_on_demand::EventLoopExtRunOnDemand;

use crate::platform::{Platform, PlatformEvent, TimeSystem, Window, WindowSystem};
// ---------------------------------------------------------------------------
// Time
// ---------------------------------------------------------------------------

pub struct DesktopTime {
    start: Instant,
    last_frame: Instant,
}

impl DesktopTime {
    pub fn new() -> Self {
        let now = Instant::now();
        Self {
            start: now,
            last_frame: now,
        }
    }
}

impl Default for DesktopTime {
    fn default() -> Self {
        Self::new()
    }
}

impl TimeSystem for DesktopTime {
    fn now(&self) -> Duration {
        Instant::now() - self.start
    }

    fn delta_seconds(&mut self) -> f32 {
        let now = Instant::now();
        let delta = now - self.last_frame;
        self.last_frame = now;
        delta.as_secs_f32()
    }
}

// ---------------------------------------------------------------------------
// Window
// ---------------------------------------------------------------------------

pub struct DesktopWindow {
    pub(crate) window: winit::window::Window,
}

// Implement HasWindowHandle from raw-window-handle 0.6 by delegating to the
// inner winit window (which also implements it).
impl HasWindowHandle for DesktopWindow {
    fn window_handle(&self) -> Result<WindowHandle<'_>, raw_window_handle::HandleError> {
        self.window.window_handle()
    }
}

impl HasDisplayHandle for DesktopWindow {
    fn display_handle(&self) -> Result<raw_window_handle::DisplayHandle<'_>, raw_window_handle::HandleError> {
        self.window.display_handle()
    }
}

impl Window for DesktopWindow {
    fn raw_window_handle(&self) -> WindowHandle<'_> {
        // winit's Window always returns a valid handle in practice.
        self.window.window_handle().expect("winit window handle is always valid in DesktopWindow")
    }

    fn size(&self) -> (u32, u32) {
        let size = self.window.inner_size();
        (size.width, size.height)
    }
}

// ---------------------------------------------------------------------------
// Send-safe wrapper for winit's EventLoop
//
// winit's X11 EventLoop is !Send because it contains raw Xlib pointers.
// In our architecture the EventLoop is created and consumed on a single
// thread, so we can safely assert Send.
// ---------------------------------------------------------------------------

struct SendEventLoop(EventLoop<()>);

// SAFETY: EventLoop is only ever accessed from a single thread.
unsafe impl Send for SendEventLoop {}

impl std::ops::Deref for SendEventLoop {
    type Target = EventLoop<()>;

    fn deref(&self) -> &EventLoop<()> {
        &self.0
    }
}

impl std::ops::DerefMut for SendEventLoop {
    fn deref_mut(&mut self) -> &mut EventLoop<()> {
        &mut self.0
    }
}

// ---------------------------------------------------------------------------
// WindowSystem
// ---------------------------------------------------------------------------

pub struct DesktopWindowSystem {
    event_loop: Option<SendEventLoop>,
}

impl WindowSystem for DesktopWindowSystem {
    #[allow(deprecated)]
    fn create_window(&mut self, title: &str, width: u32, height: u32) -> Box<dyn Window> {
        let event_loop = self
            .event_loop
            .as_ref()
            .expect("event loop already consumed");
        let window_attrs = winit::window::Window::default_attributes()
            .with_title(title)
            .with_inner_size(LogicalSize::new(f64::from(width), f64::from(height)));
        let window = event_loop  // create_window is deprecated in 0.30 but works
            .create_window(window_attrs)
            .expect("failed to create window");
        Box::new(DesktopWindow { window })
    }

    fn poll_events(&mut self) -> Vec<PlatformEvent> {
        let mut events = Vec::new();
        if let Some(el) = &mut self.event_loop {
            #[allow(deprecated)]
            let _ = el.pump_events(Some(std::time::Duration::ZERO), |event, _ael| {
                if let winit::event::Event::WindowEvent { event: we, .. } = event {
                    match we {
                        winit::event::WindowEvent::CloseRequested => {
                            events.push(PlatformEvent::CloseRequested);
                        }
                        winit::event::WindowEvent::Resized(size) => {
                            events.push(PlatformEvent::Resized(size.width, size.height));
                        }
                        _ => {}
                    }
                }
            });
        }
        events
    }

    #[allow(deprecated)]
    fn run_event_loop(&mut self, on_event: &mut dyn FnMut(PlatformEvent)) {
        let send_event_loop = self
            .event_loop
            .as_mut()
            .expect("event loop not created");

        // Box the reference and leak it to obtain a raw pointer that the
        // closure can capture.  The closure recreates the box on each
        // invocation and re-leaks it to avoid a double free.  This is
        // sound because `run_on_demand` is *synchronous* — it blocks
        // until the event loop exits — so the original `on_event` borrow
        // remains alive for the entire call.
        let on_event_ptr = Box::into_raw(Box::new(on_event));

        if let Err(e) = send_event_loop.run_on_demand(
            move |event, active_event_loop: &ActiveEventLoop| {
                // SAFETY: on_event_ptr was allocated by Box::into_raw above
                // and is still valid (we re-leak before returning).
                let on_event_box = unsafe { Box::from_raw(on_event_ptr) };
                match event {
                    Event::WindowEvent { event: we, .. } => match we {
                        WindowEvent::CloseRequested => {
                            (*on_event_box)(PlatformEvent::CloseRequested);
                            active_event_loop.exit();
                        }
                        WindowEvent::Resized(size) => {
                            (*on_event_box)(PlatformEvent::Resized(size.width, size.height));
                        }
                        _ => {}
                    },
                    Event::AboutToWait => {
                        // The user is expected to drive frame timing via
                        // `TimeSystem::delta_seconds()` separately.
                    }
                    _ => {}
                }
                // Re-leak to prevent dropping the borrowed reference.
                let _ = Box::into_raw(on_event_box);
            },
        ) {
            tracing::warn!("Event loop exited with error: {e:#}");
        }
    }
}

// ---------------------------------------------------------------------------
// Platform
// ---------------------------------------------------------------------------

pub struct DesktopPlatform {
    window_system: DesktopWindowSystem,
    time: DesktopTime,
}

impl DesktopPlatform {
    pub fn new() -> anyhow::Result<Self> {
        let mut event_loop_builder = EventLoop::builder();
        let event_loop = event_loop_builder.build()?;
        Ok(Self {
            window_system: DesktopWindowSystem {
                event_loop: Some(SendEventLoop(event_loop)),
            },
            time: DesktopTime::new(),
        })
    }
}

impl Platform for DesktopPlatform {
    fn name(&self) -> &'static str {
        "desktop"
    }

    fn window_system(&mut self) -> &mut dyn WindowSystem {
        &mut self.window_system
    }

    fn time_system(&mut self) -> &mut dyn TimeSystem {
        &mut self.time
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn desktop_time_monotonic() {
        let mut t = DesktopTime::new();
        let d1 = t.delta_seconds();
        let d2 = t.delta_seconds();
        // Both deltas should be >= 0 and very small (not deterministically ordered
        // since the second call may observe more wall time).
        assert!(d1 >= 0.0);
        assert!(d2 >= 0.0);
    }
}
