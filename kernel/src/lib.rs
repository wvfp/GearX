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

pub use init::{init_logging, init_logging_with_default, KERNEL_VERSION};
pub use frame_counter::FrameCounter;

/// Temporary forward declaration — replaced by the real `Kernel` in Task 8.
pub struct Kernel;
