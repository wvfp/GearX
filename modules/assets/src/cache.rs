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
#[derive(Debug)]
pub struct AssetCache<T> {
    assets: HashMap<u64, Arc<T>>,
    next_id: u64,
}

impl<T> AssetCache<T> {
    pub fn new() -> Self {
        Self {
            assets: HashMap::new(),
            next_id: 1,
        }
    }

    pub fn insert(&mut self, asset: T) -> AssetHandle<T> {
        let id = self.next_id;
        self.next_id += 1;
        let inner = Arc::new(asset);
        self.assets.insert(id, Arc::clone(&inner));
        AssetHandle { inner, id }
    }

    pub fn get(&self, id: u64) -> Option<AssetHandle<T>> {
        self.assets.get(&id).map(|data| AssetHandle {
            inner: Arc::clone(data),
            id,
        })
    }

    pub fn remove(&mut self, id: u64) {
        self.assets.remove(&id);
    }

    #[must_use]
    pub fn len(&self) -> usize {
        self.assets.len()
    }

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
