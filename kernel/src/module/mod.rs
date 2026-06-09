// Module trait and distributed-slice registry for GearX modules
//
// Each game engine module (render, input, audio, etc.) implements `Module`
// and registers itself via `#[distributed_slice(MODULES)]`.

use crate::Kernel;
use anyhow::Result;
use linkme::distributed_slice;

/// Trait that all GearX engine modules must implement.
pub trait Module: Send + 'static {
    /// Human-readable name of this module (e.g. "render", "input").
    fn name(&self) -> &'static str;

    /// Other module names this module depends on (empty by default).
    fn dependencies(&self) -> &[&'static str] {
        &[]
    }

    /// Load priority (higher = loaded first; 0 by default).
    fn priority(&self) -> u32 {
        0
    }

    /// One-time initialisation. Called during engine startup.
    fn init(&mut self, _kernel: &mut Kernel) -> Result<()>;

    /// Per-frame update. Called each tick of the main loop.
    fn update(&mut self, _kernel: &mut Kernel, _dt: f32) -> Result<()>;

    /// Graceful shutdown. Called during engine teardown.
    fn shutdown(&mut self, _kernel: &mut Kernel) -> Result<()>;
}

/// Distributed slice of module factory functions.
///
/// Each module crate provides a `#[distributed_slice(MODULES)]` item
/// that appends a `fn() -> Box<dyn Module>` to this list at link time.
#[distributed_slice]
pub static MODULES: [fn() -> Box<dyn Module>] = [..];

mod registry;
pub use registry::{ModuleRegistry, ModuleState};
