use std::time::Instant;

pub struct FrameCounter {
    frame_count: u64,
    last_log: Instant,
    fps: f32,
}

impl FrameCounter {
    pub fn new() -> Self {
        Self {
            frame_count: 0,
            last_log: Instant::now(),
            fps: 0.0,
        }
    }
    
    pub fn tick(&mut self) {
        self.frame_count += 1;
        
        let elapsed = self.last_log.elapsed();
        if elapsed >= std::time::Duration::from_secs(1) {
            self.fps = self.frame_count as f32 / elapsed.as_secs_f32();
            tracing::trace!("Frame {} complete — FPS: {:.1}", self.frame_count, self.fps);
            self.frame_count = 0;
            self.last_log = Instant::now();
        }
    }
    
    pub fn frame_count(&self) -> u64 { self.frame_count }
    pub fn fps(&self) -> f32 { self.fps }
}
