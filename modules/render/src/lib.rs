// GearX render module — wgpu-based graphics subsystem.
//
// Provides a `RenderModule` that initialises wgpu, manages a window surface,
// and draws a clear-colour frame each tick of the engine main loop.
//
// Because wgpu surface creation requires a window handle, this module cannot
// be auto-constructed by the linkme distributed-slice factory (the factory
// intentionally panics).  The desktop binary must create a `RenderModule`
// manually via `RenderModule::new(window)` and register it with the kernel.

use anyhow::Result;
use gearx_kernel::module::Module;
use gearx_kernel::platform::Window;
use gearx_kernel::Kernel;
use linkme::distributed_slice;

/// Top-level render module responsible for the GPU device, swap chain,
/// surface management, and frame lifecycle.
pub struct RenderModule {
    device: wgpu::Device,
    queue: wgpu::Queue,
    surface: wgpu::Surface<'static>,
    surface_config: wgpu::SurfaceConfiguration,
    window_size: (u32, u32),
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
        // `&'static dyn Window: HasWindowHandle + HasDisplayHandle` thanks
        // to the supertrait bounds on `gearx_kernel::platform::Window`, so
        // wgpu can create a surface directly.
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

        Ok(Self {
            device,
            queue,
            surface,
            surface_config,
            window_size: (width, height),
        })
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

// ── Module trait implementation ──────────────────────────────────────────

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

        // Begin a single render pass that clears the surface to black.
        {
            let _pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                label: None,
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
        }

        self.queue.submit(std::iter::once(encoder.finish()));
        frame.present();

        Ok(())
    }

    fn shutdown(&mut self, _kernel: &mut Kernel) -> Result<()> {
        // All wgpu resources (device, queue, surface) are dropped
        // automatically when `Self` is dropped.
        Ok(())
    }
}

// ── linkme distributed-slice registration ────────────────────────────────
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
