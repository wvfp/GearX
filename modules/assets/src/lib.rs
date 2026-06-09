// GearX asset management module.
//
// Provides texture and mesh caching, GPU upload helpers, and built-in
// primitive factories.  Registered via linkme so the kernel discovers it
// automatically.

use anyhow::Result;
use gearx_kernel::module::Module;
use gearx_kernel::Kernel;
use linkme::distributed_slice;

pub mod cache;
pub mod mesh;
pub mod texture;

use cache::AssetCache;
pub use mesh::{LoadedMesh, MeshLoader, Vertex};
pub use texture::{LoadedTexture, TextureLoader};

/// The asset module maintains texture and mesh caches in CPU memory.
///
/// # Initialisation
///
/// The binary should inject a [`TextureLoader`] after construction via
/// [`set_texture_loader()`](AssetModule::set_texture_loader) to enable
/// GPU-backed texture loading.
pub struct AssetModule {
    pub textures: AssetCache<LoadedTexture>,
    pub meshes: AssetCache<LoadedMesh>,
    texture_loader: Option<TextureLoader>,
}

impl AssetModule {
    /// Create a new asset module with empty caches.
    pub fn new() -> Self {
        Self {
            textures: AssetCache::new(),
            meshes: AssetCache::new(),
            texture_loader: None,
        }
    }

    /// Provide a texture loader so the module can load textures from disk.
    pub fn set_texture_loader(&mut self, loader: TextureLoader) {
        self.texture_loader = Some(loader);
    }

    /// Load a texture from file, cache it, and return a handle.
    ///
    /// Requires that [`set_texture_loader`] was called first.
    pub fn load_texture(&mut self, path: &std::path::Path) -> anyhow::Result<cache::AssetHandle<LoadedTexture>> {
        let loader = self
            .texture_loader
            .as_ref()
            .ok_or_else(|| anyhow::anyhow!("TextureLoader not set on AssetModule"))?;
        let tex = loader.load_from_file(path)?;
        Ok(self.textures.insert(tex))
    }

    /// Get or create the default white 1×1 texture.
    pub fn white_texture(&mut self) -> anyhow::Result<cache::AssetHandle<LoadedTexture>> {
        let loader = self
            .texture_loader
            .as_ref()
            .ok_or_else(|| anyhow::anyhow!("TextureLoader not set on AssetModule"))?;
        let tex = loader.white_texture()?;
        Ok(self.textures.insert(tex))
    }

    /// Load a mesh from an OBJ file, cache it, and return a handle.
    pub fn load_mesh(&mut self, path: &std::path::Path) -> anyhow::Result<cache::AssetHandle<LoadedMesh>> {
        let mesh = MeshLoader::load_from_obj(path)?;
        Ok(self.meshes.insert(mesh))
    }
}

impl Default for AssetModule {
    fn default() -> Self {
        Self::new()
    }
}

// ── Module trait ──────────────────────────────────────────────────────────

impl Module for AssetModule {
    fn name(&self) -> &'static str {
        "assets"
    }

    fn init(&mut self, _kernel: &mut Kernel) -> Result<()> {
        tracing::info!("AssetModule: ready (caches empty, no texture loader)");
        Ok(())
    }

    fn update(&mut self, _kernel: &mut Kernel, _dt: f32) -> Result<()> {
        // Passive module — no per-frame work.
        Ok(())
    }

    fn shutdown(&mut self, _kernel: &mut Kernel) -> Result<()> {
        tracing::info!("AssetModule: shutting down, {} textures cached", self.textures.len());
        Ok(())
    }
}

// ── linkme registration ───────────────────────────────────────────────────

#[distributed_slice(gearx_kernel::module::MODULES)]
pub static REGISTER_ASSETS: fn() -> Box<dyn Module> = || -> Box<dyn Module> {
    Box::new(AssetModule::new())
};
