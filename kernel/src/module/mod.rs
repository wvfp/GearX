// Module trait and distributed-slice registry for GearX modules
//
// Each game engine module (render, input, audio, etc.) implements `Module`
// and registers itself via `#[distributed_slice(MODULES)]`.

use anyhow::Result;
use linkme::distributed_slice;

/// Trait that all GearX engine modules must implement.
pub trait Module: Send + Sync {
    /// Human-readable name of this module (e.g. "render", "input").
    fn name(&self) -> &'static str;

    /// One-time initialisation. Called during engine startup.
    fn init(&mut self) -> Result<()>;

    /// Per-frame update. Called each tick of the main loop.
    fn update(&mut self) -> Result<()>;

    /// Graceful shutdown. Called during engine teardown.
    fn shutdown(&mut self) -> Result<()>;
}

/// Distributed slice of module factory functions.
///
/// Each module crate provides a `#[distributed_slice(MODULES)]` item
/// that appends a `fn() -> Box<dyn Module>` to this list at link time.
#[distributed_slice]
pub static MODULES: [fn() -> Box<dyn Module>] = [..];
