//! ECS integration — core resources for the engine kernel.
//!
//! Provides [`Time`] and [`FrameStats`] resources inserted into the kernel's
//! ECS [`World`](bevy_ecs::world::World).  These are updated each frame and
//! available to all ECS systems.

use bevy_ecs::system::Resource;

/// Frame timing resource, inserted into the ECS world at kernel startup.
///
/// # Fields
/// * `delta` — seconds since the last frame
/// * `elapsed` — total wall-clock seconds since the kernel started
/// * `frame_count` — number of frames rendered since kernel start
#[derive(Debug, Default)]
pub struct Time {
    pub delta: f32,
    pub elapsed: f32,
    pub frame_count: u64,
}

impl Resource for Time {}

/// Frame statistics resource, inserted into the ECS world at kernel startup.
///
/// # Fields
/// * `fps` — measured frames per second (updated each frame)
#[derive(Debug, Default)]
pub struct FrameStats {
    pub fps: f32,
}

impl Resource for FrameStats {}
