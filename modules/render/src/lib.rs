use std::sync::{Arc, Mutex};

use anyhow::Result;
use gearx_kernel::module::Module;
use gearx_kernel::platform::Window;
use gearx_kernel::Kernel;
use linkme::distributed_slice;
use wgpu::util::DeviceExt;

mod mesh;
pub use mesh::{Mesh, Vertex};

#[derive(Debug, Default)]
pub struct RenderOffset {
    pub x: f32,
    pub y: f32,
}

impl gearx_kernel::Resource for RenderOffset {}

// ═══════════════════════════════════════════════════════════════════════════
// Shared render context
// ═══════════════════════════════════════════════════════════════════════════

pub struct RenderContext {
    pub device: Arc<wgpu::Device>,
    pub queue: Arc<wgpu::Queue>,
    pub surface_format: wgpu::TextureFormat,
    pub screen_size: Arc<Mutex<(u32, u32)>>,
}

impl gearx_kernel::Resource for RenderContext {}

/// Shared egui tessellation data written by UiModule, read by RenderModule.
pub struct EguiRenderData {
    pub primitives: Vec<egui::ClippedPrimitive>,
    pub screen_descriptor: egui_wgpu::ScreenDescriptor,
}

impl gearx_kernel::Resource for EguiRenderData {}

// ═══════════════════════════════════════════════════════════════════════════
// Embedded WGSL shaders
// ═══════════════════════════════════════════════════════════════════════════

const VERTEX_SHADER_SRC: &str = r#"
struct Transform {
    offset: vec3f,
    _pad: f32,
};
@group(0) @binding(0) var t_diffuse: texture_2d<f32>;
@group(0) @binding(1) var s_diffuse: sampler;
@group(1) @binding(0) var<uniform> transform: Transform;
struct VertexInput {
    @location(0) position: vec3f,
    @location(1) uv: vec2f,
    @location(2) color: vec4f,
};
struct VertexOutput {
    @builtin(position) clip_position: vec4f,
    @location(0) uv: vec2f,
    @location(1) color: vec4f,
};
@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.clip_position = vec4f(in.position + transform.offset, 1.0);
    out.uv = in.uv;
    out.color = in.color;
    return out;
}
"#;

const FRAGMENT_SHADER_SRC: &str = r#"
struct VertexOutput {
    @builtin(position) clip_position: vec4f,
    @location(0) uv: vec2f,
    @location(1) color: vec4f,
};
@group(0) @binding(0) var t_diffuse: texture_2d<f32>;
@group(0) @binding(1) var s_diffuse: sampler;
@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let tex_color = textureSample(t_diffuse, s_diffuse, in.uv);
    return tex_color * in.color;
}
"#;

#[repr(C)]
#[derive(Debug, Clone, Copy)]
struct TransformUniform {
    offset: [f32; 3],
    _pad: f32,
}

unsafe impl bytemuck::Pod for TransformUniform {}
unsafe impl bytemuck::Zeroable for TransformUniform {}

// ═══════════════════════════════════════════════════════════════════════════
// RenderModule
// ═══════════════════════════════════════════════════════════════════════════

pub struct RenderModule {
    device: Arc<wgpu::Device>,
    queue: Arc<wgpu::Queue>,
    surface: wgpu::Surface<'static>,
    surface_config: wgpu::SurfaceConfiguration,
    window_size: (u32, u32),
    screen_size_shared: Arc<Mutex<(u32, u32)>>,
    render_pipeline: wgpu::RenderPipeline,
    vertex_buffer: wgpu::Buffer,
    index_buffer: wgpu::Buffer,
    num_indices: u32,
    texture_bind_group: wgpu::BindGroup,
    transform_buffer: wgpu::Buffer,
    transform_bind_group: wgpu::BindGroup,
    egui_renderer: egui_wgpu::Renderer,
}

impl RenderModule {
    pub async fn new(window: &'static dyn Window) -> Result<Self> {
        let instance = wgpu::Instance::new(&wgpu::InstanceDescriptor {
            backends: wgpu::Backends::PRIMARY,
            ..Default::default()
        });
        let surface = instance.create_surface(window)
            .map_err(|e| anyhow::anyhow!("failed to create wgpu surface: {e}"))?;
        let adapter = instance.request_adapter(&wgpu::RequestAdapterOptions {
            power_preference: wgpu::PowerPreference::HighPerformance,
            compatible_surface: Some(&surface),
            ..Default::default()
        }).await.ok_or_else(|| anyhow::anyhow!("no suitable GPU adapter found"))?;
        let (device, queue) = adapter.request_device(&wgpu::DeviceDescriptor::default(), None).await?;
        let (width, height) = window.size();
        let surface_config = surface.get_default_config(&adapter, width, height)
            .ok_or_else(|| anyhow::anyhow!("failed to get default surface config"))?;
        surface.configure(&device, &surface_config);

        let device = Arc::new(device);
        let queue = Arc::new(queue);
        let screen_size_shared = Arc::new(Mutex::new((width, height)));

        // Default 1×1 white texture
        let tex_size = wgpu::Extent3d { width: 1, height: 1, depth_or_array_layers: 1 };
        let default_texture = device.create_texture(&wgpu::TextureDescriptor {
            label: Some("Default White Texture"), size: tex_size, mip_level_count: 1, sample_count: 1,
            dimension: wgpu::TextureDimension::D2, format: wgpu::TextureFormat::Rgba8UnormSrgb,
            usage: wgpu::TextureUsages::TEXTURE_BINDING, view_formats: &[],
        });
        queue.write_texture(
            wgpu::TexelCopyTextureInfo { texture: &default_texture, mip_level: 0, origin: wgpu::Origin3d::ZERO, aspect: wgpu::TextureAspect::All },
            &[255, 255, 255, 255],
            wgpu::TexelCopyBufferLayout { offset: 0, bytes_per_row: Some(4), rows_per_image: Some(1) },
            tex_size,
        );
        let default_view = default_texture.create_view(&wgpu::TextureViewDescriptor::default());
        let default_sampler = device.create_sampler(&wgpu::SamplerDescriptor {
            label: Some("Default Sampler"), mag_filter: wgpu::FilterMode::Linear, min_filter: wgpu::FilterMode::Linear, ..Default::default()
        });

        let texture_bgl = device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
            label: Some("Texture BGL"),
            entries: &[
                wgpu::BindGroupLayoutEntry { binding: 0, visibility: wgpu::ShaderStages::FRAGMENT,
                    ty: wgpu::BindingType::Texture { multisampled: false, view_dimension: wgpu::TextureViewDimension::D2, sample_type: wgpu::TextureSampleType::Float { filterable: true } }, count: None },
                wgpu::BindGroupLayoutEntry { binding: 1, visibility: wgpu::ShaderStages::FRAGMENT,
                    ty: wgpu::BindingType::Sampler(wgpu::SamplerBindingType::Filtering), count: None },
            ],
        });
        let texture_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            label: Some("Texture Bind Group"), layout: &texture_bgl,
            entries: &[
                wgpu::BindGroupEntry { binding: 0, resource: wgpu::BindingResource::TextureView(&default_view) },
                wgpu::BindGroupEntry { binding: 1, resource: wgpu::BindingResource::Sampler(&default_sampler) },
            ],
        });

        let transform_bgl = device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
            label: Some("Transform BGL"),
            entries: &[wgpu::BindGroupLayoutEntry { binding: 0, visibility: wgpu::ShaderStages::VERTEX,
                ty: wgpu::BindingType::Buffer { ty: wgpu::BufferBindingType::Uniform, has_dynamic_offset: false, min_binding_size: None }, count: None }],
        });
        let transform_uniform = TransformUniform { offset: [0.0; 3], _pad: 0.0 };
        let transform_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Transform Uniform Buffer"),
            contents: bytemuck::cast_slice(&[transform_uniform]),
            usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
        });
        let transform_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            label: Some("Transform Bind Group"), layout: &transform_bgl,
            entries: &[wgpu::BindGroupEntry { binding: 0, resource: transform_buffer.as_entire_binding() }],
        });

        let render_pipeline = Self::create_pipeline(&device, &surface_config, &texture_bgl, &transform_bgl)?;
        let mesh = Mesh::triangle();
        let (vertex_buffer, index_buffer, num_indices) = Self::upload_mesh(&device, &mesh);

        let egui_renderer = egui_wgpu::Renderer::new(&*device, surface_config.format, None, 1, false);

        tracing::info!("RenderModule — pipeline created, mesh uploaded ({} verts, {} indices)", mesh.vertices.len(), mesh.indices.len());

        Ok(Self {
            device, queue, surface, surface_config, window_size: (width, height), screen_size_shared,
            render_pipeline, vertex_buffer, index_buffer, num_indices,
            texture_bind_group, transform_buffer, transform_bind_group, egui_renderer,
        })
    }

    fn create_shader_module(device: &wgpu::Device, label: &str, source: &str) -> wgpu::ShaderModule {
        device.create_shader_module(wgpu::ShaderModuleDescriptor {
            label: Some(label), source: wgpu::ShaderSource::Wgsl(std::borrow::Cow::Borrowed(source)),
        })
    }

    fn create_pipeline(device: &wgpu::Device, config: &wgpu::SurfaceConfiguration,
        texture_bgl: &wgpu::BindGroupLayout, transform_bgl: &wgpu::BindGroupLayout,
    ) -> Result<wgpu::RenderPipeline> {
        let vs = Self::create_shader_module(device, "Default VS", VERTEX_SHADER_SRC);
        let fs = Self::create_shader_module(device, "Default FS", FRAGMENT_SHADER_SRC);
        let layout = device.create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
            label: Some("Default Pipeline Layout"), bind_group_layouts: &[texture_bgl, transform_bgl], push_constant_ranges: &[],
        });
        let vertex_size = std::mem::size_of::<Vertex>() as wgpu::BufferAddress;
        let pipeline = device.create_render_pipeline(&wgpu::RenderPipelineDescriptor {
            label: Some("Default Render Pipeline"), layout: Some(&layout),
            vertex: wgpu::VertexState { module: &vs, entry_point: Some("vs_main"),
                compilation_options: wgpu::PipelineCompilationOptions::default(),
                buffers: &[wgpu::VertexBufferLayout { array_stride: vertex_size, step_mode: wgpu::VertexStepMode::Vertex,
                    attributes: &[
                        wgpu::VertexAttribute { format: wgpu::VertexFormat::Float32x3, offset: 0, shader_location: 0 },
                        wgpu::VertexAttribute { format: wgpu::VertexFormat::Float32x2, offset: 12, shader_location: 1 },
                        wgpu::VertexAttribute { format: wgpu::VertexFormat::Float32x4, offset: 20, shader_location: 2 },
                    ] }] },
            fragment: Some(wgpu::FragmentState { module: &fs, entry_point: Some("fs_main"),
                compilation_options: wgpu::PipelineCompilationOptions::default(),
                targets: &[Some(wgpu::ColorTargetState { format: config.format, blend: Some(wgpu::BlendState::REPLACE), write_mask: wgpu::ColorWrites::ALL })] }),
            primitive: wgpu::PrimitiveState { topology: wgpu::PrimitiveTopology::TriangleList, ..Default::default() },
            depth_stencil: None, multisample: wgpu::MultisampleState::default(), multiview: None, cache: None,
        });
        Ok(pipeline)
    }

    fn upload_mesh(device: &wgpu::Device, mesh: &Mesh) -> (wgpu::Buffer, wgpu::Buffer, u32) {
        let vb = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Vertex Buffer"), contents: bytemuck::cast_slice(&mesh.vertices), usage: wgpu::BufferUsages::VERTEX,
        });
        let ib = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Index Buffer"), contents: bytemuck::cast_slice(&mesh.indices), usage: wgpu::BufferUsages::INDEX,
        });
        (vb, ib, mesh.indices.len() as u32)
    }

    pub fn set_mesh(&mut self, mesh: &Mesh) {
        let (vb, ib, ni) = Self::upload_mesh(&self.device, mesh);
        self.vertex_buffer = vb; self.index_buffer = ib; self.num_indices = ni;
    }

    pub fn resize(&mut self, width: u32, height: u32) {
        if width == 0 || height == 0 { return; }
        self.window_size = (width, height);
        self.surface_config.width = width;
        self.surface_config.height = height;
        self.surface.configure(&self.device, &self.surface_config);
        if let Ok(mut size) = self.screen_size_shared.lock() { *size = (width, height); }
    }
}

impl Module for RenderModule {
    fn name(&self) -> &'static str { "render" }

    fn init(&mut self, kernel: &mut Kernel) -> Result<()> {
        kernel.add_resource(RenderOffset::default());
        kernel.add_resource(RenderContext {
            device: Arc::clone(&self.device), queue: Arc::clone(&self.queue),
            surface_format: self.surface_config.format, screen_size: Arc::clone(&self.screen_size_shared),
        });
        kernel.add_resource(EguiRenderData {
            primitives: Vec::new(),
            screen_descriptor: egui_wgpu::ScreenDescriptor {
                size_in_pixels: [self.window_size.0, self.window_size.1],
                pixels_per_point: 1.0,
            },
        });
        Ok(())
    }

    fn update(&mut self, kernel: &mut Kernel, _dt: f32) -> Result<()> {
        if self.window_size.0 == 0 || self.window_size.1 == 0 { return Ok(()); }

        if let Some(offset) = kernel.world().get_resource::<RenderOffset>() {
            let t = TransformUniform { offset: [offset.x, offset.y, 0.0], _pad: 0.0 };
            self.queue.write_buffer(&self.transform_buffer, 0, bytemuck::cast_slice(&[t]));
        }

        // Update egui textures/buffers from EguiRenderData.
        if let Some(data) = kernel.world().get_resource::<EguiRenderData>() {
            let clipped = &data.primitives;
            if !clipped.is_empty() {
                let mut enc = self.device.create_command_encoder(&wgpu::CommandEncoderDescriptor::default());
                self.egui_renderer.update_buffers(&*self.device, &*self.queue, &mut enc, clipped, &data.screen_descriptor);
                self.queue.submit(std::iter::once(enc.finish()));
            }
        }

        let frame = self.surface.get_current_texture()?;
        let view = frame.texture.create_view(&wgpu::TextureViewDescriptor::default());
        let mut encoder = self.device.create_command_encoder(&wgpu::CommandEncoderDescriptor::default());

        {
            let mut pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                label: Some("Main Render Pass"),
                color_attachments: &[Some(wgpu::RenderPassColorAttachment {
                    view: &view, resolve_target: None,
                    ops: wgpu::Operations { load: wgpu::LoadOp::Clear(wgpu::Color::BLACK), store: wgpu::StoreOp::Store },
                })],
                depth_stencil_attachment: None, occlusion_query_set: None, timestamp_writes: None,
            });

            pass.set_pipeline(&self.render_pipeline);
            pass.set_bind_group(0, &self.texture_bind_group, &[]);
            pass.set_bind_group(1, &self.transform_bind_group, &[]);
            pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
            pass.set_index_buffer(self.index_buffer.slice(..), wgpu::IndexFormat::Uint16);
            pass.draw_indexed(0..self.num_indices, 0, 0..1);

            // Egui overlay.
            if let Some(data) = kernel.world().get_resource::<EguiRenderData>() {
                if !data.primitives.is_empty() {
                    // SAFETY: egui_wgpu::Renderer::render requires RenderPass<'static> but
                    // our pass borrows from a local encoder. This is safe because the pass
                    // is dropped before the encoder is used again (finish/submit).
                    let pass_static: &mut wgpu::RenderPass<'static> = unsafe { std::mem::transmute(&mut pass) };
                    self.egui_renderer.render(pass_static, &data.primitives, &data.screen_descriptor);
                }
            }
        }

        self.queue.submit(std::iter::once(encoder.finish()));
        frame.present();
        Ok(())
    }

    fn shutdown(&mut self, _kernel: &mut Kernel) -> Result<()> { Ok(()) }
}

#[distributed_slice(gearx_kernel::module::MODULES)]
pub static REGISTER_RENDER: fn() -> Box<dyn Module> = || -> Box<dyn Module> {
    panic!("RenderModule cannot be auto-constructed without a window; use `RenderModule::new(window)` and register it manually");
};
