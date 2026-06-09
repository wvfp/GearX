use std::sync::atomic::{AtomicBool, AtomicUsize, Ordering};
use std::sync::Arc;

use super::{Event, EventBus, EventHandler};

// ── Test Event Types ──

#[derive(Clone, Debug)]
struct TestEvent(u32);
impl Event for TestEvent {}

#[derive(Clone, Debug)]
struct EventA;
impl Event for EventA {}

#[derive(Clone, Debug)]
struct EventB;
impl Event for EventB {}

// ── Handler helpers ──

/// Handler that sets an AtomicBool to true when called.
struct FlagHandler(Arc<AtomicBool>);

impl<E: Event> EventHandler<E> for FlagHandler {
    fn handle(&mut self, _event: &E) {
        self.0.store(true, Ordering::SeqCst);
    }
}

/// Handler that increments an AtomicUsize counter.
struct CountHandler(Arc<AtomicUsize>);

impl<E: Event> EventHandler<E> for CountHandler {
    fn handle(&mut self, _event: &E) {
        self.0.fetch_add(1, Ordering::SeqCst);
    }
}

// ── Tests ──

#[test]
fn test_publish_calls_handler() {
    let mut bus = EventBus::new();
    let flag = Arc::new(AtomicBool::new(false));

    bus.listen::<TestEvent, _>(FlagHandler(flag.clone()));
    bus.publish(&TestEvent(42));

    assert!(flag.load(Ordering::SeqCst), "handler should have been called");
}

#[test]
fn test_multiple_handlers() {
    let mut bus = EventBus::new();
    let count1 = Arc::new(AtomicUsize::new(0));
    let count2 = Arc::new(AtomicUsize::new(0));
    let count3 = Arc::new(AtomicUsize::new(0));

    bus.listen::<TestEvent, _>(CountHandler(count1.clone()));
    bus.listen::<TestEvent, _>(CountHandler(count2.clone()));
    bus.listen::<TestEvent, _>(CountHandler(count3.clone()));
    bus.publish(&TestEvent(99));

    assert_eq!(count1.load(Ordering::SeqCst), 1);
    assert_eq!(count2.load(Ordering::SeqCst), 1);
    assert_eq!(count3.load(Ordering::SeqCst), 1);
}

#[test]
fn test_unrelated_event_not_called() {
    let mut bus = EventBus::new();
    let flag = Arc::new(AtomicBool::new(false));

    // Register handler for EventA only
    bus.listen::<EventA, _>(FlagHandler(flag.clone()));

    // Publish EventB — handler should NOT fire
    bus.publish(&EventB);

    assert!(!flag.load(Ordering::SeqCst), "handler for EventA should not be called for EventB");
}

#[test]
fn test_handler_removed() {
    let mut bus = EventBus::new();
    let flag = Arc::new(AtomicBool::new(false));

    bus.listen::<TestEvent, _>(FlagHandler(flag.clone()));
    bus.remove_all::<TestEvent>();
    bus.publish(&TestEvent(0));

    assert!(!flag.load(Ordering::SeqCst), "handler should not be called after removal");
}

#[test]
fn test_publish_no_handler_no_panic() {
    let mut bus = EventBus::new();

    // Publish an event type that has no registered handlers — must not panic.
    bus.publish(&EventA);
    bus.publish(&EventB);
    bus.publish(&TestEvent(7));
    // If we reach here, no panic occurred.
}

/// Compile-time assertion that all event types implement `Send`.
#[test]
fn test_event_trait_send() {
    fn assert_send<T: Send>() {}
    assert_send::<TestEvent>();
    assert_send::<EventA>();
    assert_send::<EventB>();
}
