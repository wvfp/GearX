use std::sync::{Arc, Mutex};

use anyhow::Result;
use gearx_kernel::event::{EventHandler, Event};
use gearx_kernel::module::Module;
use gearx_kernel::platform::PlatformEvent;
use gearx_kernel::Kernel;
use linkme::distributed_slice;

// ── Key Code Enum ──

/// Keyboard key codes.
///
/// Discriminant values match `winit::keyboard::KeyCode` (USB HID usage IDs).
#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
#[repr(u32)]
pub enum KeyCode {
    KeyA = 0x0004,
    KeyD = 0x0007,
    KeyS = 0x0016,
    KeyW = 0x001A,
    Space = 0x002C,
    Enter = 0x0028,
    Escape = 0x0029,
    ArrowDown = 0x0054,
    ArrowLeft = 0x0055,
    ArrowRight = 0x0056,
    ArrowUp = 0x0057,
}

impl KeyCode {
    /// Map a winit physical keycode (u32) to a `KeyCode`, or `None` if unmapped.
    pub fn from_scancode(scancode: u32) -> Option<KeyCode> {
        match scancode {
            0x0004 => Some(KeyCode::KeyA),
            0x0007 => Some(KeyCode::KeyD),
            0x0016 => Some(KeyCode::KeyS),
            0x001A => Some(KeyCode::KeyW),
            0x002C => Some(KeyCode::Space),
            0x0028 => Some(KeyCode::Enter),
            0x0029 => Some(KeyCode::Escape),
            0x0054 => Some(KeyCode::ArrowDown),
            0x0055 => Some(KeyCode::ArrowLeft),
            0x0056 => Some(KeyCode::ArrowRight),
            0x0057 => Some(KeyCode::ArrowUp),
            _ => None,
        }
    }
}

// ── Mouse Button Enum ──

/// Mouse buttons (subset of winit's `MouseButton`).
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum MouseButton {
    Left,
    Right,
    Middle,
}

fn mouse_button_from_u8(b: u8) -> Option<MouseButton> {
    match b {
        0 => Some(MouseButton::Left),
        1 => Some(MouseButton::Right),
        2 => Some(MouseButton::Middle),
        _ => None,
    }
}

// ── Input Events ──

/// Events produced by the input module.
#[derive(Clone, Debug)]
pub enum InputEvent {
    KeyDown { key: KeyCode },
    KeyUp { key: KeyCode },
    MouseMove { x: f32, y: f32 },
    MouseDown { button: MouseButton },
    MouseUp { button: MouseButton },
}

impl Event for InputEvent {}

// ── Input State ──

/// ECS resource tracking current input state.
pub struct InputState {
    pub keys_pressed: [bool; 256],
    pub mouse_pos: (f32, f32),
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

impl gearx_kernel::Resource for InputState {}

// ── Platform → Input Bridge ──

type PendingInputQueue = Arc<Mutex<Vec<InputEvent>>>;

/// Subscribes to `PlatformEvent` on the EventBus and converts keyboard/mouse
/// events into `InputEvent`s stored in a shared queue.  `InputModule::update()`
/// drains the queue each frame.
struct PlatformInputBridge {
    pending: PendingInputQueue,
}

impl EventHandler<PlatformEvent> for PlatformInputBridge {
    fn handle(&mut self, event: &PlatformEvent) {
        let input_event = match event {
            PlatformEvent::KeyPressed(sc) => {
                KeyCode::from_scancode(*sc).map(|key| InputEvent::KeyDown { key })
            }
            PlatformEvent::KeyReleased(sc) => {
                KeyCode::from_scancode(*sc).map(|key| InputEvent::KeyUp { key })
            }
            PlatformEvent::CursorMoved(x, y) => {
                Some(InputEvent::MouseMove { x: *x, y: *y })
            }
            PlatformEvent::MouseButtonPressed(b) => {
                mouse_button_from_u8(*b).map(|button| InputEvent::MouseDown { button })
            }
            PlatformEvent::MouseButtonReleased(b) => {
                mouse_button_from_u8(*b).map(|button| InputEvent::MouseUp { button })
            }
            _ => None,
        };
        if let Some(ie) = input_event {
            if let Ok(mut q) = self.pending.lock() {
                q.push(ie);
            }
        }
    }
}

// ── Input Module ──

/// Top-level input module — tracks keyboard/mouse state via ECS resource.
pub struct InputModule {
    state: InputState,
    pending: PendingInputQueue,
}

impl InputModule {
    pub fn new() -> Self {
        Self {
            state: InputState::default(),
            pending: Arc::new(Mutex::new(Vec::new())),
        }
    }

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

    #[must_use]
    pub fn state(&self) -> &InputState {
        &self.state
    }

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

    fn init(&mut self, kernel: &mut Kernel) -> Result<()> {
        kernel.add_resource(InputState::default());

        let bridge = PlatformInputBridge {
            pending: Arc::clone(&self.pending),
        };
        kernel.event_bus().listen::<PlatformEvent, _>(bridge);
        Ok(())
    }

    fn update(&mut self, kernel: &mut Kernel, _dt: f32) -> Result<()> {
        let events: Vec<InputEvent> = {
            let mut q = self.pending.lock().expect("input queue lock");
            q.drain(..).collect()
        };
        for event in &events {
            self.handle_event(event);
        }

        // Sync internal state to ECS resource.
        if let Some(mut res) = kernel.world().get_resource_mut::<InputState>() {
            res.keys_pressed = self.state.keys_pressed;
            res.mouse_pos = self.state.mouse_pos;
            res.mouse_buttons = self.state.mouse_buttons;
        }
        Ok(())
    }

    fn shutdown(&mut self, _kernel: &mut Kernel) -> Result<()> {
        Ok(())
    }
}

// ── linkme registration ──

#[distributed_slice(gearx_kernel::module::MODULES)]
pub static REGISTER_INPUT: fn() -> Box<dyn Module> = || -> Box<dyn Module> {
    Box::new(InputModule::new())
};

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn handle_key_down() {
        let mut module = InputModule::new();
        module.handle_event(&InputEvent::KeyDown { key: KeyCode::ArrowUp });
        assert!(module.state().keys_pressed[KeyCode::ArrowUp as usize]);
    }

    #[test]
    fn handle_key_up() {
        let mut module = InputModule::new();
        module.handle_event(&InputEvent::KeyDown { key: KeyCode::ArrowUp });
        module.handle_event(&InputEvent::KeyUp { key: KeyCode::ArrowUp });
        assert!(!module.state().keys_pressed[KeyCode::ArrowUp as usize]);
    }

    #[test]
    fn handle_mouse_move() {
        let mut module = InputModule::new();
        module.handle_event(&InputEvent::MouseMove { x: 100.0, y: 200.0 });
        assert_eq!(module.state().mouse_pos, (100.0, 200.0));
    }

    #[test]
    fn handle_mouse_buttons() {
        let mut module = InputModule::new();
        module.handle_event(&InputEvent::MouseDown { button: MouseButton::Left });
        assert!(module.state().mouse_buttons[0]);
        module.handle_event(&InputEvent::MouseUp { button: MouseButton::Left });
        assert!(!module.state().mouse_buttons[0]);
    }

    #[test]
    fn default_state_all_released() {
        let state = InputState::default();
        assert!(state.keys_pressed.iter().all(|&k| !k));
        assert!(state.mouse_buttons.iter().all(|&b| !b));
    }

    #[test]
    fn scancode_mapping() {
        assert_eq!(KeyCode::from_scancode(0x0057), Some(KeyCode::ArrowUp));
        assert_eq!(KeyCode::from_scancode(0x0054), Some(KeyCode::ArrowDown));
        assert_eq!(KeyCode::from_scancode(0x0004), Some(KeyCode::KeyA));
        assert_eq!(KeyCode::from_scancode(0xFFFF), None);
    }
}
