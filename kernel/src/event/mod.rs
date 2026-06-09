use std::any::{Any, TypeId};
use std::collections::HashMap;

// ── Event Traits ──

/// Marker trait for all events that can be published on the EventBus.
pub trait Event: Send + 'static {}

/// Handler trait — implement for each event type you want to receive.
pub trait EventHandler<E: Event>: Send + 'static {
    fn handle(&mut self, event: &E);
}

// ── Type-erased handler storage ──

pub trait AnyHandler: Send + 'static {
    fn handle_as_any(&mut self, event: &dyn Any);
}

// ── EventBus ──

/// Synchronous in-process event bus.
///
/// Routes events by their `TypeId` to registered handlers.
/// All handlers are invoked synchronously during `publish()`.
pub struct EventBus {
    handlers: HashMap<TypeId, Vec<Box<dyn AnyHandler>>>,
}

impl EventBus {
    pub fn new() -> Self {
        Self {
            handlers: HashMap::new(),
        }
    }

    /// Register a handler for event type `E`.
    pub fn listen<E: Event, H: EventHandler<E> + 'static>(&mut self, handler: H) {
        let type_id = TypeId::of::<E>();
        let wrapped = WrappedHandler::<E, H>::new(handler);
        self.handlers
            .entry(type_id)
            .or_default()
            .push(Box::new(wrapped));
    }

    /// Synchronously dispatch `event` to all registered handlers.
    pub fn publish<E: Event>(&mut self, event: &E) {
        let type_id = TypeId::of::<E>();
        if let Some(handlers) = self.handlers.get_mut(&type_id) {
            for handler in handlers {
                handler.handle_as_any(event as &dyn Any);
            }
        }
    }

    /// Remove all handlers registered for event type `E`.
    pub fn remove_all<E: Event>(&mut self) {
        self.handlers.remove(&TypeId::of::<E>());
    }

    /// Remove all handlers for every event type.
    pub fn clear(&mut self) {
        self.handlers.clear();
    }
}

impl Default for EventBus {
    fn default() -> Self {
        Self::new()
    }
}

// ── WrappedHandler (type-erased bridge) ──

struct WrappedHandler<E: Event, H: EventHandler<E>> {
    handler: H,
    _marker: std::marker::PhantomData<E>,
}

impl<E: Event, H: EventHandler<E>> WrappedHandler<E, H> {
    fn new(handler: H) -> Self {
        Self {
            handler,
            _marker: std::marker::PhantomData,
        }
    }
}

impl<E: Event, H: EventHandler<E> + 'static> AnyHandler for WrappedHandler<E, H> {
    fn handle_as_any(&mut self, event: &dyn Any) {
        if let Some(e) = event.downcast_ref::<E>() {
            self.handler.handle(e);
        }
    }
}

// ── Pre-defined Event Types ──

#[derive(Clone, Copy, Debug, PartialEq)]
pub enum EngineMode {
    Edit,
    Play,
    Pause,
}

/// Events related to the engine lifecycle.
#[derive(Clone, Debug)]
pub enum EngineEvent {
    ModulesLoaded,
    ModuleLoaded(&'static str),
    ModuleUnloaded(&'static str),
    Shutdown,
    ModeChanged(EngineMode),
}

/// Events related to the per-frame update loop.
#[derive(Clone, Debug)]
pub enum FrameEvent {
    Start(f32), // dt in seconds
    End(f32),   // dt in seconds
}

/// Input events — not yet populated (Phase 2+).
#[derive(Clone, Debug)]
pub enum InputEvent {
    // Placeholder — no variants in Phase 1
}

// ── Blanket Event impls ──

impl Event for EngineEvent {}
impl Event for FrameEvent {}
impl Event for InputEvent {}

#[cfg(test)]
mod tests;
