use std::collections::VecDeque;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum LogLevel {
    Info,
    Warn,
    Error,
    Debug,
}

#[derive(Clone, Debug)]
pub struct LogEntry {
    pub level: LogLevel,
    pub message: String,
}

/// In-memory log buffer for engine diagnostics.
pub struct Console {
    entries: VecDeque<LogEntry>,
    max_entries: usize,
}

impl Console {
    pub fn new() -> Self {
        Self {
            entries: VecDeque::new(),
            max_entries: 256,
        }
    }

    pub fn log(&mut self, level: LogLevel, message: impl Into<String>) {
        if self.entries.len() >= self.max_entries {
            self.entries.pop_front();
        }
        self.entries.push_back(LogEntry {
            level,
            message: message.into(),
        });
    }

    #[must_use]
    pub fn entries(&self) -> &VecDeque<LogEntry> {
        &self.entries
    }

    #[must_use]
    pub fn len(&self) -> usize {
        self.entries.len()
    }

    #[must_use]
    pub fn is_empty(&self) -> bool {
        self.entries.is_empty()
    }

    pub fn clear(&mut self) {
        self.entries.clear();
    }
}

impl Default for Console {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn log_adds_entries() {
        let mut c = Console::new();
        c.log(LogLevel::Info, "hello");
        c.log(LogLevel::Warn, "world");
        assert_eq!(c.len(), 2);
    }

    #[test]
    fn evicts_oldest_at_capacity() {
        let mut c = Console { max_entries: 3, ..Console::new() };
        c.log(LogLevel::Info, "a");
        c.log(LogLevel::Info, "b");
        c.log(LogLevel::Info, "c");
        c.log(LogLevel::Info, "d");
        assert_eq!(c.len(), 3);
        assert_eq!(c.entries()[0].message, "b");
    }

    #[test]
    fn clear_empties_buffer() {
        let mut c = Console::new();
        c.log(LogLevel::Error, "x");
        c.clear();
        assert!(c.is_empty());
    }
}
