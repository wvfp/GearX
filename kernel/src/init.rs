/// Initialize tracing subscriber with default settings (stderr output, timestamps, RUST_LOG env)
pub fn init_logging() {
    tracing_subscriber::fmt()
        .with_env_filter(
            tracing_subscriber::EnvFilter::from_default_env()
        )
        .init();
}

/// Initialize tracing subscriber with a fallback level if RUST_LOG is not set
pub fn init_logging_with_default(default_level: &str) {
    let filter = match std::env::var("RUST_LOG") {
        Ok(val) => tracing_subscriber::EnvFilter::new(val),
        Err(_) => tracing_subscriber::EnvFilter::new(default_level),
    };
    
    tracing_subscriber::fmt()
        .with_env_filter(filter)
        .init();
}

/// Kernel version constant
pub const KERNEL_VERSION: &str = env!("CARGO_PKG_VERSION");
