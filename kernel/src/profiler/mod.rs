use std::collections::HashMap;
use std::time::Instant;

/// Per-frame performance profiler with module-level timing.
pub struct FrameProfiler {
    frame_start: Option<Instant>,
    frame_time_ms: f64,
    module_times: HashMap<&'static str, f64>,
    current_module_start: Option<Instant>,
    frame_count: u64,
}

impl FrameProfiler {
    pub fn new() -> Self {
        Self {
            frame_start: None,
            frame_time_ms: 0.0,
            module_times: HashMap::new(),
            current_module_start: None,
            frame_count: 0,
        }
    }

    pub fn begin_frame(&mut self) {
        self.frame_start = Some(Instant::now());
        self.module_times.clear();
    }

    pub fn end_frame(&mut self) {
        if let Some(start) = self.frame_start.take() {
            self.frame_time_ms = start.elapsed().as_secs_f64() * 1000.0;
        }
        self.frame_count += 1;
    }

    pub fn begin_module(&mut self, _name: &'static str) {
        self.current_module_start = Some(Instant::now());
    }

    pub fn end_module(&mut self, name: &'static str) {
        if let Some(start) = self.current_module_start.take() {
            self.module_times.insert(name, start.elapsed().as_secs_f64() * 1000.0);
        }
    }

    #[must_use]
    pub fn frame_time_ms(&self) -> f64 {
        self.frame_time_ms
    }

    #[must_use]
    pub fn module_time_ms(&self, name: &str) -> Option<f64> {
        self.module_times.get(name).copied()
    }

    #[must_use]
    pub fn frame_count(&self) -> u64 {
        self.frame_count
    }

    /// Produce a human-readable report of the last frame.
    #[must_use]
    pub fn report(&self) -> String {
        let mut s = format!("Frame {} — {:.2}ms total", self.frame_count, self.frame_time_ms);
        for (name, ms) in &self.module_times {
            s.push_str(&format!(", {}: {:.2}ms", name, ms));
        }
        s
    }
}

impl Default for FrameProfiler {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn begin_end_frame_records_time() {
        let mut p = FrameProfiler::new();
        p.begin_frame();
        std::thread::sleep(std::time::Duration::from_millis(5));
        p.end_frame();
        assert!(p.frame_time_ms() > 0.0);
        assert_eq!(p.frame_count(), 1);
    }

    #[test]
    fn module_timing() {
        let mut p = FrameProfiler::new();
        p.begin_frame();
        p.begin_module("render");
        std::thread::sleep(std::time::Duration::from_millis(2));
        p.end_module("render");
        p.end_frame();
        assert!(p.module_time_ms("render").unwrap() > 0.0);
        assert!(p.module_time_ms("nonexistent").is_none());
    }
}
