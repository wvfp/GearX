//! Texture loading and GPU upload.

use anyhow::Result;
use image::GenericImageView;
use std::path::Path;

/// A texture that has been uploaded to the GPU and is ready for rendering.
#[derive(Debug)]
pub struct LoadedTexture {
    pub texture: wgpu::Texture,
    pub view: wgpu::TextureView,
    pub sampler: wgpu::Sampler,
    pub size: (u32, u32),
}

/// Loads texture data from disk (PNG/JPEG) and uploads it to the GPU.
pub struct TextureLoader {
    device: wgpu::Device,
    queue: wgpu::Queue,
}

impl TextureLoader {
    /// Create a loader with the given GPU device and command queue.
    pub fn new(device: wgpu::Device, queue: wgpu::Queue) -> Self {
        Self { device, queue }
    }

    /// Load a texture from a PNG or JPEG file on disk.
    ///
    /// # Errors
    ///
    /// Returns an error if the file cannot be read or the image format is
    /// not recognised.
    pub fn load_from_file(&self, path: &Path) -> Result<LoadedTexture> {
        let img = image::open(path)?;
        let dimensions = img.dimensions();
        let rgba = img.to_rgba8();
        self.upload_rgba(dimensions.0, dimensions.1, &rgba)
    }

    /// Create a 1×1 white texture (useful as a default / fallback).
    pub fn white_texture(&self) -> Result<LoadedTexture> {
        let pixel: [u8; 4] = [255, 255, 255, 255];
        self.upload_rgba(1, 1, &pixel)
    }

    // ── internal ────────────────────────────────────────────────────────

    fn upload_rgba(&self, width: u32, height: u32, data: &[u8]) -> Result<LoadedTexture> {
        let size = wgpu::Extent3d {
            width,
            height,
            depth_or_array_layers: 1,
        };

        let texture = self.device.create_texture(&wgpu::TextureDescriptor {
            label: Some("Loaded Texture"),
            size,
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: wgpu::TextureFormat::Rgba8UnormSrgb,
            usage: wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
            view_formats: &[],
        });

        self.queue.write_texture(
            wgpu::TexelCopyTextureInfo {
                texture: &texture,
                mip_level: 0,
                origin: wgpu::Origin3d::ZERO,
                aspect: wgpu::TextureAspect::All,
            },
            data,
            wgpu::TexelCopyBufferLayout {
                offset: 0,
                bytes_per_row: Some(4 * width),
                rows_per_image: Some(height),
            },
            size,
        );

        let view = texture.create_view(&wgpu::TextureViewDescriptor::default());
        let sampler = self.device.create_sampler(&wgpu::SamplerDescriptor {
            label: Some("Texture Sampler"),
            address_mode_u: wgpu::AddressMode::ClampToEdge,
            address_mode_v: wgpu::AddressMode::ClampToEdge,
            address_mode_w: wgpu::AddressMode::ClampToEdge,
            mag_filter: wgpu::FilterMode::Linear,
            min_filter: wgpu::FilterMode::Linear,
            mipmap_filter: wgpu::FilterMode::Linear,
            ..Default::default()
        });

        Ok(LoadedTexture {
            texture,
            view,
            sampler,
            size: (width, height),
        })
    }
}