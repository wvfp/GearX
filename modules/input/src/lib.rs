// GearX input module — keyboard and mouse input handling.
//
// Manages `InputState` (an ECS resource in Phase 2+) and processes
// `InputEvent`s from the platform layer.  The module registers itself
// via linkme so it is automatically discovered at link time.
//
// The input module is passive during init: it does not subscribe to the
// EventBus because the platform layer (T1) has not yet been wired to
// publish keyboard/mouse events.  Instead, external code pushes events
// through `InputModule::handle_event()`, which updates `InputState`.

use anyhow::Result;
use gearx_kernel::module::Module;
use gearx_kernel::Kernel;
use linkme::distributed_slice;

// ── Key Code Enum ──

/// Keyboard key codes.
///
/// These mirror a subset of `winit::keyboard::KeyCode` but are defined
/// locally to avoid a direct dependency on winit.  The platform layer
/// maps winit scancodes into these values.
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub enum KeyCode {
    ArrowUp,
    ArrowDown,
    ArrowLeft,
    ArrowRight,
    KeyW,
    KeyA,
    KeyS,
    KeyD,
    Space,
    Enter,
    Escape,
}

// ── Mouse Button Enum ──

/// Mouse buttons (subset of winit's `MouseButton`).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum MouseButton {
    /// Primary (usually left) button.
    Left,
    /// Secondary (usually right) button.
    Right,
    /// Middle (wheel) button.
    Middle,
}

// ── Input Events ──

/// Events produced by the input module.
#[derive(Clone, Debug)]
pub enum InputEvent {
    /// A keyboard key was pressed.
    KeyDown { key: KeyCode },
    /// A keyboard key was released.
    KeyUp { key: KeyCode },
    /// The mouse was moved to a new position (window coordinates).
    MouseMove { x: f32, y: f32 },
    /// A mouse button was pressed.
    MouseDown { button: MouseButton },
    /// A mouse button was released.
    MouseUp { button: MouseButton },
}

// Allow `InputEvent` to be routed through the kernel's EventBus.
impl gearx_kernel::event::Event for InputEvent {}

// ── Input State ──

/// ECS resource tracking current input state.
///
/// This struct will be registered as an ECS resource in Phase 2+.
/// For now it is simply a plain Rust struct owned by `InputModule`.
pub struct InputState {
    /// Keyboard keys currently pressed (indexed by `KeyCode` as `u32`).
    pub keys_pressed: [bool; 256],
    /// Mouse position in window coordinates.
    pub mouse_pos: (f32, f32),
    /// Mouse buttons currently pressed (indexed by `u8`: 0=Left, 1=Right, 2=Middle).
    pub mouse_buttons: [bool; 3],
}

impl Default for InputState {
    fn default() -> Self {
        Self {
            keys_pressed: [false; 256],
            mouse_pos: (0.0, 0.0),
            mouse_buttons: [false; 3],
        }
    }
}

// ── Input Module ──

/// Top-level input module responsible for tracking input state.
pub struct InputModule {
    state: InputState,
}

impl InputModule {
    /// Create a new `InputModule` with default (all-released) state.
    pub fn new() -> Self {
        Self {
            state: InputState::default(),
        }
    }

    /// Process a single `InputEvent` and update the internal state accordingly.
    ///
    /// Called by the platform layer (or any external code) each time a
    /// keyboard or mouse event occurs.
    pub fn handle_event(&mut self, event: &InputEvent) {
        match event {
            InputEvent::KeyDown { key } => {
                self.state.keys_pressed[*key as usize] = true;
            }
            InputEvent::KeyUp { key } => {
                self.state.keys_pressed[*key as usize] = false;
            }
            InputEvent::MouseMove { x, y } => {
                self.state.mouse_pos = (*x, *y);
            }
            InputEvent::MouseDown { button } => {
                self.state.mouse_buttons[*button as usize] = true;
            }
            InputEvent::MouseUp { button } => {
                self.state.mouse_buttons[*button as usize] = false;
            }
        }
    }

    /// Returns a reference to the current input state.
    #[must_use]
    pub fn state(&self) -> &InputState {
        &self.state
    }

    /// Returns a mutable reference to the current input state.
    pub fn state_mut(&mut self) -> &mut InputState {
        &mut self.state
    }
}

impl Default for InputModule {
    fn default() -> Self {
        Self::new()
    }
}

// ── Module trait implementation ──

impl Module for InputModule {
    fn name(&self) -> &'static str {
        "input"
    }

    fn init(&mut self, _kernel: &mut Kernel) -> Result<()> {
        // TODO (Phase 2): Subscribe to InputEvent on the EventBus once the
        // platform layer publishes keyboard/mouse events.  For now the
        // module is passive; platform code calls `handle_event()` directly.
        Ok(())
    }

    fn update(&mut self, _kernel: &mut Kernel, _dt: f32) -> Result<()> {
        // Input state is updated incrementally by `handle_event()` calls
        // from the platform layer or EventBus handlers.  No per-frame
        // work needed.
        Ok(())
    }

    fn shutdown(&mut self, _kernel: &mut Kernel) -> Result<()> {
        // All input state is dropped when `Self` is dropped.
        Ok(())
    }
}

// ── linkme distributed-slice registration ──
//
// Registers a factory function so the input module is auto-discovered
// when `ModuleRegistry::discover()` is called.

#[distributed_slice(gearx_kernel::module::MODULES)]
pub static REGISTER_INPUT: fn() -> Box<dyn Module> = || -> Box<dyn Module> {
    Box::new(InputModule::new())
};
