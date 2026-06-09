// GearX render module — wgpu-based graphics subsystem.
//
// Provides a `RenderModule` that initialises wgpu, manages a window surface,
// compiles WGSL shaders, creates a rendering pipeline, and draws meshes
// (triangle → quad → cube) each frame.

use anyhow::Result;
use gearx_kernel::module::Module;
use gearx_kernel::platform::Window;
use gearx_kernel::Kernel;
use linkme::distributed_slice;
use wgpu::util::DeviceExt;

mod mesh;
pub use mesh::{Mesh, Vertex};

// ═══════════════════════════════════════════════════════════════════════════
// Embedded WGSL shaders
// ═══════════════════════════════════════════════════════════════════════════

const VERTEX_SHADER_SRC: &str = r#"
struct VertexInput {
    @location(0) position: vec3f,
    @location(1) color: vec4f,
};

struct VertexOutput {
    @builtin(position) clip_position: vec4f,
    @location(0) color: vec4f,
};

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.clip_position = vec4f(in.position, 1.0);
    out.color = in.color;
    return out;
}
"#;

const FRAGMENT_SHADER_SRC: &str = r#"
@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    return in.color;
}
"#;

// ═══════════════════════════════════════════════════════════════════════════
// RenderModule
// ═══════════════════════════════════════════════════════════════════════════

/// Top-level render module responsible for the GPU device, swap chain,
/// pipeline, and frame lifecycle.
pub struct RenderModule {
    device: wgpu::Device,
    queue: wgpu::Queue,
    surface: wgpu::Surface<'static>,
    surface_config: wgpu::SurfaceConfiguration,
    window_size: (u32, u32),
    render_pipeline: wgpu::RenderPipeline,
    vertex_buffer: wgpu::Buffer,
    index_buffer: wgpu::Buffer,
    num_indices: u32,
}

impl RenderModule {
    /// Asynchronously initialise wgpu from a leaked `'static` window reference.
    ///
    /// The caller must ensure the window is leaked (e.g. via `Box::leak`) so
    /// that the surface can borrow it for the `'static` lifetime required by the
    /// `Module` trait.
    pub async fn new(window: &'static dyn Window) -> Result<Self> {
        // ── Instance ────────────────────────────────────────────────────
        let instance = wgpu::Instance::new(&wgpu::InstanceDescriptor {
            backends: wgpu::Backends::PRIMARY,
            ..Default::default()
        });

        // ── Surface ─────────────────────────────────────────────────────
        let surface = instance
            .create_surface(window)
            .map_err(|e| anyhow::anyhow!("failed to create wgpu surface: {e}"))?;

        // ── Adapter ─────────────────────────────────────────────────────
        let adapter = instance
            .request_adapter(&wgpu::RequestAdapterOptions {
                power_preference: wgpu::PowerPreference::HighPerformance,
                compatible_surface: Some(&surface),
                ..Default::default()
            })
            .await
            .ok_or_else(|| anyhow::anyhow!("no suitable GPU adapter found"))?;

        // ── Device & queue ──────────────────────────────────────────────
        let (device, queue) = adapter
            .request_device(&wgpu::DeviceDescriptor::default(), None)
            .await?;

        // ── Surface configuration ───────────────────────────────────────
        let (width, height) = window.size();
        let surface_config = surface
            .get_default_config(&adapter, width, height)
            .ok_or_else(|| anyhow::anyhow!("failed to get default surface config"))?;
        surface.configure(&device, &surface_config);

        // ── Shaders & pipeline ──────────────────────────────────────────
        let render_pipeline = Self::create_pipeline(&device, &surface_config)?;
        let mesh = Mesh::triangle();
        let (vertex_buffer, index_buffer, num_indices) =
            Self::upload_mesh(&device, &queue, &mesh);

        tracing::info!(
            "RenderModule — pipeline created, mesh '{}' uploaded ({} verts, {} indices)",
            "triangle",
            mesh.vertices.len(),
            mesh.indices.len(),
        );

        Ok(Self {
            device,
            queue,
            surface,
            surface_config,
            window_size: (width, height),
            render_pipeline,
            vertex_buffer,
            index_buffer,
            num_indices,
        })
    }

    // ── Pipeline creation ───────────────────────────────────────────────

    fn create_shader_module(device: &wgpu::Device, label: &str, source: &str) -> wgpu::ShaderModule {
        device.create_shader_module(wgpu::ShaderModuleDescriptor {
            label: Some(label),
            source: wgpu::ShaderSource::Wgsl(std::borrow::Cow::Borrowed(source)),
        })
    }

    fn create_pipeline(
        device: &wgpu::Device,
        config: &wgpu::SurfaceConfiguration,
    ) -> Result<wgpu::RenderPipeline> {
        let vs_module = Self::create_shader_module(device, "Default VS", VERTEX_SHADER_SRC);
        let fs_module = Self::create_shader_module(device, "Default FS", FRAGMENT_SHADER_SRC);

        let pipeline_layout = device.create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
            label: Some("Default Pipeline Layout"),
            bind_group_layouts: &[],
            push_constant_ranges: &[],
        });

        let vertex_size = std::mem::size_of::<Vertex>() as wgpu::BufferAddress;

        let pipeline =
            device.create_render_pipeline(&wgpu::RenderPipelineDescriptor {
                label: Some("Default Render Pipeline"),
                layout: Some(&pipeline_layout),
                vertex: wgpu::VertexState {
                    module: &vs_module,
                    entry_point: Some("vs_main"),
                    compilation_options: wgpu::PipelineCompilationOptions::default(),
                    buffers: &[wgpu::VertexBufferLayout {
                        array_stride: vertex_size,
                        step_mode: wgpu::VertexStepMode::Vertex,
                        attributes: &[
                            wgpu::VertexAttribute {
                                format: wgpu::VertexFormat::Float32x3,
                                offset: 0,
                                shader_location: 0,
                            },
                            wgpu::VertexAttribute {
                                format: wgpu::VertexFormat::Float32x4,
                                offset: 12, // after 3 × f32
                                shader_location: 1,
                            },
                        ],
                    }],
                },
                fragment: Some(wgpu::FragmentState {
                    module: &fs_module,
                    entry_point: Some("fs_main"),
                    compilation_options: wgpu::PipelineCompilationOptions::default(),
                    targets: &[Some(wgpu::ColorTargetState {
                        format: config.format,
                        blend: Some(wgpu::BlendState::REPLACE),
                        write_mask: wgpu::ColorWrites::ALL,
                    })],
                }),
                primitive: wgpu::PrimitiveState {
                    topology: wgpu::PrimitiveTopology::TriangleList,
                    ..Default::default()
                },
                depth_stencil: None,
                multisample: wgpu::MultisampleState::default(),
                multiview: None,
                cache: None,
            });

        Ok(pipeline)
    }

    // ── Mesh upload ─────────────────────────────────────────────────────

    fn upload_mesh(
        device: &wgpu::Device,
        _queue: &wgpu::Queue,
        mesh: &Mesh,
    ) -> (wgpu::Buffer, wgpu::Buffer, u32) {
        let vertex_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Vertex Buffer"),
            contents: bytemuck::cast_slice(&mesh.vertices),
            usage: wgpu::BufferUsages::VERTEX,
        });
        let index_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Index Buffer"),
            contents: bytemuck::cast_slice(&mesh.indices),
            usage: wgpu::BufferUsages::INDEX,
        });
        (vertex_buffer, index_buffer, mesh.indices.len() as u32)
    }

    /// Replace the currently displayed mesh with a new one.
    pub fn set_mesh(&mut self, mesh: &Mesh) {
        let (vb, ib, ni) = Self::upload_mesh(&self.device, &self.queue, mesh);
        self.vertex_buffer = vb;
        self.index_buffer = ib;
        self.num_indices = ni;
        tracing::debug!("RenderModule: mesh updated ({} indices)", ni);
    }

    /// Handle a window resize event by reconfiguring the wgpu surface.
    /// If either dimension is zero (window minimised), the resize is deferred.
    pub fn resize(&mut self, width: u32, height: u32) {
        if width == 0 || height == 0 {
            tracing::warn!("RenderModule: skipping resize to ({width}×{height}) — window minimised");
            return;
        }
        self.window_size = (width, height);
        self.surface_config.width = width;
        self.surface_config.height = height;
        self.surface.configure(&self.device, &self.surface_config);
        tracing::info!("RenderModule: surface resized to {width}×{height}");
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Module trait implementation
// ═══════════════════════════════════════════════════════════════════════════

impl Module for RenderModule {
    fn name(&self) -> &'static str {
        "render"
    }

    fn init(&mut self, _kernel: &mut Kernel) -> Result<()> {
        // Initialisation already completed in `new()`.  Nothing to do here.
        Ok(())
    }

    fn update(&mut self, _kernel: &mut Kernel, _dt: f32) -> Result<()> {
        // Skip rendering while the window is minimised (zero-sized).
        if self.window_size.0 == 0 || self.window_size.1 == 0 {
            return Ok(());
        }

        // Acquire the next swap-chain texture.
        let frame = self.surface.get_current_texture()?;
        let view = frame
            .texture
            .create_view(&wgpu::TextureViewDescriptor::default());

        let mut encoder =
            self.device
                .create_command_encoder(&wgpu::CommandEncoderDescriptor::default());

        {
            let mut pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                label: Some("Main Render Pass"),
                color_attachments: &[Some(wgpu::RenderPassColorAttachment {
                    view: &view,
                    resolve_target: None,
                    ops: wgpu::Operations {
                        load: wgpu::LoadOp::Clear(wgpu::Color::BLACK),
                        store: wgpu::StoreOp::Store,
                    },
                })],
                depth_stencil_attachment: None,
                occlusion_query_set: None,
                timestamp_writes: None,
            });

            // Draw the current mesh.
            pass.set_pipeline(&self.render_pipeline);
            pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
            pass.set_index_buffer(self.index_buffer.slice(..), wgpu::IndexFormat::Uint16);
            pass.draw_indexed(0..self.num_indices, 0, 0..1);
        }

        self.queue.submit(std::iter::once(encoder.finish()));
        frame.present();

        Ok(())
    }

    fn shutdown(&mut self, _kernel: &mut Kernel) -> Result<()> {
        // All wgpu resources are dropped automatically when `Self` is dropped.
        Ok(())
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// linkme distributed-slice registration
// ═══════════════════════════════════════════════════════════════════════════
//
// The factory panics because `RenderModule` requires a window handle that
// cannot be provided through the no-argument factory signature.  The desktop
// binary creates the module manually with `RenderModule::new()`.

#[distributed_slice(gearx_kernel::module::MODULES)]
pub static REGISTER_RENDER: fn() -> Box<dyn Module> = || -> Box<dyn Module> {
    panic!(
        "RenderModule cannot be auto-constructed without a window; \
         use `RenderModule::new(window)` and register it manually"
    );
};
