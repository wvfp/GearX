//! Generic asset cache with reference counting.
//!
//! Stores assets behind `Arc` pointers and hands out lightweight handles.
//! Assets are evicted when all handles are dropped and the cache is
//! explicitly trimmed.

use std::collections::HashMap;
use std::sync::Arc;

/// A reference-counted handle to a cached asset.
#[derive(Debug, Clone)]
pub struct AssetHandle<T> {
    inner: Arc<T>,
    pub id: u64,
}

impl<T> AssetHandle<T> {
    pub fn new(inner: T, id: u64) -> Self {
        Self {
            inner: Arc::new(inner),
            id,
        }
    }

    pub fn get(&self) -> &T {
        &self.inner
    }
}

/// Cache of typed assets identified by numeric IDs.
///
/// Internally uses `Arc` so cloning a handle is cheap and the asset lives
/// as long as any handle exists.
#[derive(Debug)]
pub struct AssetCache<T> {
    assets: HashMap<u64, AssetEntry<T>>,
    next_id: u64,
}

#[derive(Debug)]
struct AssetEntry<T> {
    data: Arc<T>,
    strong_refs: usize,
}

impl<T> AssetCache<T> {
    /// Create an empty cache.
    pub fn new() -> Self {
        Self {
            assets: HashMap::new(),
            next_id: 1,
        }
    }

    /// Insert a new asset and return a handle to it.
    pub fn insert(&mut self, asset: T) -> AssetHandle<T> {
        let id = self.next_id;
        self.next_id += 1;
        let inner = Arc::new(asset);
        self.assets.insert(
            id,
            AssetEntry {
                data: Arc::clone(&inner),
                strong_refs: 1,
            },
        );
        AssetHandle { inner, id }
    }

    /// Retrieve a handle by ID.  Returns `None` if the ID was never
    /// inserted or has been removed.
    pub fn get(&self, id: u64) -> Option<AssetHandle<T>>
    where
        T: Clone,
    {
        self.assets.get(&id).map(|entry| AssetHandle {
            inner: Arc::clone(&entry.data),
            id,
        })
    }

    /// Remove an asset from the cache.  Existing handles remain valid
    /// until dropped.
    pub fn remove(&mut self, id: u64) {
        self.assets.remove(&id);
    }

    /// Number of cached assets.
    #[must_use]
    pub fn len(&self) -> usize {
        self.assets.len()
    }

    /// Whether the cache is empty.
    #[must_use]
    pub fn is_empty(&self) -> bool {
        self.assets.is_empty()
    }
}

impl<T> Default for AssetCache<T> {
    fn default() -> Self {
        Self::new()
    }
}