//! Task system — parallel work dispatch for the engine kernel.
//!
//! Phase 2 provides a lightweight [`TaskSystem`] that routes ECS schedule
//! execution through `Schedule::run()`.  Full parallel task spawning is
//! reserved for Phase 3+.

use bevy_ecs::prelude::{Schedule, World};

/// Lightweight wrapper around the ECS schedule runner.
///
/// In Phase 2 this is deliberately minimal — it records the available
/// parallelism level and delegates to `Schedule::run()`, which internally
/// executes systems with whatever parallelism `bevy_ecs` provides.
pub struct TaskSystem {
    thread_count: usize,
}

impl TaskSystem {
    /// Create a new task system that reports `thread_count` worker threads.
    pub fn new(thread_count: usize) -> Self {
        Self { thread_count }
    }

    /// Execute the ECS schedule against the given world.
    ///
    /// Systems are run in the order they were added to the schedule.
    /// `bevy_ecs` may parallelise independent systems internally.
    pub fn run_schedule(&self, world: &mut World, schedule: &mut Schedule) {
        schedule.run(world);
    }

    /// Number of hardware threads this system was configured for.
    #[must_use]
    pub fn thread_count(&self) -> usize {
        self.thread_count
    }
}

impl Default for TaskSystem {
    fn default() -> Self {
        let count = std::thread::available_parallelism()
            .map(|n| n.get())
            .unwrap_or(1);
        Self::new(count)
    }
}