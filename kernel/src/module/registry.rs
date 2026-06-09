/// Module registry — discovers, loads, and manages engine modules.
///
/// Each module is registered via `register()` or auto-discovered from the
/// [`MODULES`](crate::MODULES) distributed slice.  The registry owns all
/// module instances and tracks their lifecycle state.

use crate::{module::Module, module::MODULES, Kernel};
use anyhow::{anyhow, Result};
use std::collections::HashMap;

/// Lifecycle state of a registered module.
#[derive(Clone, Copy, Debug, PartialEq)]
pub enum ModuleState {
    /// Factory called, `init()` not yet invoked.
    Registered,
    /// `init()` completed successfully; module is running.
    Active,
    /// `shutdown()` completed; module is stopped.
    Unloaded,
    /// `init()` or `update()` returned an error.
    Error,
}

struct ModuleEntry {
    module: Box<dyn Module>,
    state: ModuleState,
}

/// Owns all registered module instances and manages their lifecycle.
///
/// # Ordering
///
/// Modules are loaded in the order they were registered (no topological
/// sort).  They are unloaded in reverse order.
pub struct ModuleRegistry {
    entries: Vec<ModuleEntry>,
    by_name: HashMap<&'static str, usize>,
}

impl ModuleRegistry {
    /// Create an empty registry.
    pub fn new() -> Self {
        Self {
            entries: Vec::new(),
            by_name: HashMap::new(),
        }
    }

    /// Register a single module instance.
    ///
    /// Panics if a module with the same name is already registered.
    pub fn register(&mut self, module: Box<dyn Module>) {
        let name = module.name();
        assert!(
            !self.by_name.contains_key(name),
            "module '{}' is already registered",
            name,
        );
        let idx = self.entries.len();
        self.by_name.insert(name, idx);
        self.entries.push(ModuleEntry {
            module,
            state: ModuleState::Registered,
        });
    }

    /// Discover and register every module linked into the
    /// [`MODULES`](crate::MODULES) distributed slice.
    pub fn discover(&mut self) {
        for factory in MODULES {
            self.register(factory());
        }
    }

    /// Initialise every registered module in registration order.
    ///
    /// On error the already-initialised modules are shut down before the
    /// error is propagated.
    pub fn load_all(&mut self, kernel: &mut Kernel) -> Result<()> {
        let count = self.entries.len();
        // Number of entries that successfully initialised (for rollback).
        let mut loaded = 0usize;

        for i in 0..count {
            // Determine the result and capture the module name *before* the
            // rollback borrows self.entries again.
            let (success, module_name) = {
                let entry = &mut self.entries[i];
                match entry.module.init(kernel) {
                    Ok(()) => {
                        entry.state = ModuleState::Active;
                        (true, "")
                    }
                    Err(_e) => {
                        entry.state = ModuleState::Error;
                        (false, entry.module.name())
                    }
                }
            };

            if success {
                loaded = i + 1;
            } else {
                // Roll back every already-active module.
                for j in (0..loaded).rev() {
                    let prev = &mut self.entries[j];
                    if let Err(e) = prev.module.shutdown(kernel) {
                        tracing::warn!("Module '{}' shutdown error during rollback: {e:#}", prev.module.name());
                    }
                    prev.state = ModuleState::Unloaded;
                }
                return Err(anyhow!("module '{module_name}' failed to init"));
            }
        }

        Ok(())
    }

    /// Shut down every active module in reverse registration order.
    pub fn unload_all(&mut self, kernel: &mut Kernel) -> Result<()> {
        let mut last_err = None;

        for entry in self.entries.iter_mut().rev() {
            if entry.state == ModuleState::Active {
                if let Err(e) = entry.module.shutdown(kernel) {
                    entry.state = ModuleState::Error;
                    last_err = Some(anyhow!("module '{}' shutdown error: {e:#}", entry.module.name()));
                } else {
                    entry.state = ModuleState::Unloaded;
                }
            }
        }

        match last_err {
            Some(e) => Err(e),
            None => Ok(()),
        }
    }

    /// Borrow a module by name.
    pub fn get(&self, name: &str) -> Option<&dyn Module> {
        let idx = *self.by_name.get(name)?;
        Some(&*self.entries[idx].module)
    }

    /// Mutably borrow a module by name.
    pub fn get_mut(&mut self, name: &str) -> Option<&mut dyn Module> {
        let idx = *self.by_name.get(name)?;
        Some(&mut *self.entries[idx].module)
    }

    /// Iterate over all registered modules mutably.
    pub fn iter_mut(&mut self) -> impl Iterator<Item = &mut Box<dyn Module>> {
        self.entries.iter_mut().map(|e| &mut e.module)
    }

    /// Number of registered modules.
    #[must_use]
    pub fn len(&self) -> usize {
        self.entries.len()
    }

    /// Returns `true` if no modules are registered.
    #[must_use]
    pub fn is_empty(&self) -> bool {
        self.entries.is_empty()
    }

    /// State of a registered module (if present).
    #[must_use]
    pub fn state(&self, name: &str) -> Option<ModuleState> {
        let idx = *self.by_name.get(name)?;
        Some(self.entries[idx].state)
    }
}

impl Default for ModuleRegistry {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    struct TestModule {
        name: &'static str,
    }

    impl Module for TestModule {
        fn name(&self) -> &'static str {
            self.name
        }

        fn init(&mut self, _kernel: &mut Kernel) -> Result<()> {
            Ok(())
        }

        fn update(&mut self, _kernel: &mut Kernel, _dt: f32) -> Result<()> {
            Ok(())
        }

        fn shutdown(&mut self, _kernel: &mut Kernel) -> Result<()> {
            Ok(())
        }
    }

    fn make_test(name: &'static str) -> Box<dyn Module> {
        Box::new(TestModule { name })
    }

    #[test]
    fn register_and_get() {
        let mut reg = ModuleRegistry::new();
        reg.register(make_test("a"));
        reg.register(make_test("b"));
        assert_eq!(reg.len(), 2);
        assert!(reg.get("a").is_some());
        assert!(reg.get("b").is_some());
        assert!(reg.get("c").is_none());
    }

    #[test]
    fn load_and_unload() {
        let mut reg = ModuleRegistry::new();
        reg.register(make_test("a"));
        reg.register(make_test("b"));

        let mut kernel = Kernel::test();
        reg.load_all(&mut kernel).unwrap();
        assert_eq!(reg.state("a"), Some(ModuleState::Active));
        assert_eq!(reg.state("b"), Some(ModuleState::Active));

        reg.unload_all(&mut kernel).unwrap();
        assert_eq!(reg.state("a"), Some(ModuleState::Unloaded));
        assert_eq!(reg.state("b"), Some(ModuleState::Unloaded));
    }

    #[test]
    fn init_failure_rolls_back() {
        struct FailModule;

        impl Module for FailModule {
            fn name(&self) -> &'static str {
                "fail"
            }
            fn init(&mut self, _kernel: &mut Kernel) -> Result<()> {
                Err(anyhow!("oops"))
            }
            fn update(&mut self, _kernel: &mut Kernel, _dt: f32) -> Result<()> {
                Ok(())
            }
            fn shutdown(&mut self, _kernel: &mut Kernel) -> Result<()> {
                Ok(())
            }
        }

        let mut reg = ModuleRegistry::new();
        reg.register(make_test("good"));
        reg.register(Box::new(FailModule));
        reg.register(make_test("never_reached"));

        let mut kernel = Kernel::test();
        assert!(reg.load_all(&mut kernel).is_err());
        // "good" should have been rolled back to Unloaded.
        assert_eq!(reg.state("good"), Some(ModuleState::Unloaded));
        // "fail" is Error.
        assert_eq!(reg.state("fail"), Some(ModuleState::Error));
        // "never_reached" stays Registered.
        assert_eq!(reg.state("never_reached"), Some(ModuleState::Registered));
    }

    #[test]
    #[should_panic(expected = "already registered")]
    fn duplicate_register_panics() {
        let mut reg = ModuleRegistry::new();
        reg.register(make_test("dup"));
        reg.register(make_test("dup"));
    }
}
