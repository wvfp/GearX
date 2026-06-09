// GearX render module — wgpu-based graphics subsystem.
//
// Initialised and driven by the engine main loop via the `Module` trait.

#![allow(unused)]

use gearx_kernel::module::Module;

/// Top-level render module responsible for the GPU device, swap chain,
/// surface management, and frame lifecycle.
pub struct RenderModule {
    // Fields will be added in later tasks (wgpu device, queue, surface, etc.)
}

impl Module for RenderModule {
    fn name(&self) -> &'static str {
        "render"
    }

    fn init(&mut self) -> anyhow::Result<()> {
        todo!("RenderModule::init — initialise wgpu device & surface (Task 9)")
    }

    fn update(&mut self) -> anyhow::Result<()> {
        todo!("RenderModule::update — acquire frame, record commands, present")
    }

    fn shutdown(&mut self) -> anyhow::Result<()> {
        todo!("RenderModule::shutdown — flush device, drop resources")
    }
}

// ── linkme distributed-slice registration ──────────────────────────────────

use gearx_kernel::module::MODULES;
use linkme::distributed_slice;

#[distributed_slice(MODULES)]
fn register_render() -> Box<dyn Module> {
    Box::new(RenderModule {})
}
