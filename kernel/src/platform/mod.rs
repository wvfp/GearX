use std::time::Duration;
use raw_window_handle::{HasDisplayHandle, HasWindowHandle, WindowHandle};

/// Events that the platform layer can emit to the application.
#[derive(Clone, Debug)]
pub enum PlatformEvent {
    /// The user requested the window to close.
    CloseRequested,
    /// The window was resized to the given (width, height).
    Resized(u32, u32),
    /// Frame-tick with the frame delta in seconds.
    Tick(f32),
    /// A keyboard key was pressed (winit physical keycode as u32).
    KeyPressed(u32),
    /// A keyboard key was released (winit physical keycode as u32).
    KeyReleased(u32),
    /// The mouse cursor moved to (x, y) in window coordinates.
    CursorMoved(f32, f32),
    /// A mouse button was pressed (0=left, 1=right, 2=middle).
    MouseButtonPressed(u8),
    /// A mouse button was released (0=left, 1=right, 2=middle).
    MouseButtonReleased(u8),
    /// Mouse scroll wheel delta (x, y). Positive y = scroll up.
    MouseWheel(f32, f32),
    /// Unicode text input from IME or keyboard.
    TextInput(String),
    /// Keyboard modifier state changed.
    ModifiersChanged { shift: bool, ctrl: bool, alt: bool, logo: bool },
    /// Window gained or lost focus.
    Focused(bool),
    /// Monitor DPI scale factor changed.
    ScaleFactorChanged(f64),
}

impl crate::event::Event for PlatformEvent {}

/// A handle to a display window.
pub trait Window: HasWindowHandle + HasDisplayHandle + Send + Sync + 'static {
    /// Returns a platform-specific raw window handle (for graphics APIs).
    fn raw_window_handle(&self) -> WindowHandle<'_>;
    /// Returns the current inner size of the window in physical pixels.
    fn size(&self) -> (u32, u32);
}

/// Manages windows and drives the platform event loop.
pub trait WindowSystem: Send + 'static {
    /// Creates a new window with the given title and dimensions.
    fn create_window(&mut self, title: &str, width: u32, height: u32) -> Box<dyn Window>;
    /// Runs the event loop, dispatching [`PlatformEvent`]s to the provided callback.
    ///
    /// This call blocks until the event loop exits (e.g. on `CloseRequested`).
    fn run_event_loop(&mut self, on_event: &mut dyn FnMut(PlatformEvent));
    /// Polls for pending platform events without blocking (non-blocking pump).
    ///
    /// Returns a list of [`PlatformEvent`]s that occurred since the last poll.
    /// The default implementation returns an empty vector, meaning no events
    /// were observed.
    fn poll_events(&mut self) -> Vec<PlatformEvent> {
        Vec::new()
    }
}

/// Provides wall-clock timing and frame delta queries.
pub trait TimeSystem: Send + 'static {
    /// Returns the duration since some fixed epoch (e.g. platform start).
    fn now(&self) -> Duration;
    /// Returns the time elapsed since the last call to `delta_seconds`.
    fn delta_seconds(&mut self) -> f32;
}

// --- Placeholder enums for future extension ---

pub enum FileSystemOp {}
pub enum ClipboardOp {}

// --- Platform trait ---

/// Top-level platform abstraction.
///
/// Implementors provide access to the window system, time system, and optional
/// file-system / clipboard access.
pub trait Platform: Send + 'static {
    /// A human-readable name for the current platform (e.g. "desktop", "android").
    fn name(&self) -> &'static str;
    /// Returns a mutable reference to the window system.
    fn window_system(&mut self) -> &mut dyn WindowSystem;
    /// Returns a mutable reference to the time system.
    fn time_system(&mut self) -> &mut dyn TimeSystem;
    /// Optional: file-system I/O.
    fn file_system(&mut self) -> Option<&mut dyn FileSystem> {
        None
    }
    /// Optional: clipboard access.
    fn clipboard(&mut self) -> Option<&mut dyn Clipboard> {
        None
    }
}

/// Trait for platform-level file I/O (reserved for future use).
pub trait FileSystem: Send + 'static {}

/// Trait for platform-level clipboard access (reserved for future use).
pub trait Clipboard: Send + 'static {}

// --- Backend modules ---

/// Desktop (winit-based) platform implementation.
pub mod desktop;

/// Android platform stub.
#[cfg(target_os = "android")]
pub mod android;

/// Web (WASM) platform stub.
#[cfg(target_arch = "wasm32")]
pub mod web;
