pub mod module;
pub mod ecs;
pub mod event;
pub mod task;
pub mod resource;
pub mod math;
pub mod platform;
pub mod profiler;
pub mod console;
pub mod init;
pub mod frame_counter;
pub mod scene;

pub use init::{init_logging, init_logging_with_default, KERNEL_VERSION};
pub use frame_counter::FrameCounter;

pub mod kernel;

pub use kernel::Kernel;

// Re-export ECS primitives so downstream crates don't need to depend on
// bevy_ecs directly.
pub use bevy_ecs::prelude::*;
pub use ecs::{FrameStats, Time};
