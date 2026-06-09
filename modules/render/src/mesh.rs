//! Mesh primitives for the GearX render module.
//!
//! Provides [`Vertex`] and [`Mesh`] types with factory methods for common
//! geometric primitives (triangle, quad, cube).

/// A single vertex: position (vec3) + color (vec4).
///
/// GPU layout (WGSL @location):
///   0 — position: vec3f  (offset 0, 12 bytes)
///   1 — color:    vec4f  (offset 12, 16 bytes)
///   total stride: 28 bytes
#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub struct Vertex {
    pub position: [f32; 3],
    pub color: [f32; 4],
}

// SAFETY: `Vertex` is `repr(C)` with only plain-old-data fields.
unsafe impl bytemuck::Pod for Vertex {}
unsafe impl bytemuck::Zeroable for Vertex {}

/// A GPU-uploadable mesh defined by a list of vertices and indices.
#[derive(Debug, Clone)]
pub struct Mesh {
    pub vertices: Vec<Vertex>,
    pub indices: Vec<u16>,
}

impl Mesh {
    /// A colourful triangle spanning the upper NDC viewport.
    pub fn triangle() -> Self {
        Self {
            vertices: vec![
                Vertex { position: [0.0, 0.5, 0.0],  color: [1.0, 0.0, 0.0, 1.0] }, // top – red
                Vertex { position: [-0.5, -0.5, 0.0], color: [0.0, 1.0, 0.0, 1.0] }, // bl – green
                Vertex { position: [0.5, -0.5, 0.0],  color: [0.0, 0.0, 1.0, 1.0] }, // br – blue
            ],
            indices: vec![0, 1, 2],
        }
    }

    /// A white quad covering roughly the center of the screen.
    pub fn quad() -> Self {
        let half: f32 = 0.5;
        Self {
            vertices: vec![
                Vertex { position: [-half, -half, 0.0], color: [1.0; 4] },
                Vertex { position: [ half, -half, 0.0], color: [1.0; 4] },
                Vertex { position: [ half,  half, 0.0], color: [1.0; 4] },
                Vertex { position: [-half,  half, 0.0], color: [1.0; 4] },
            ],
            indices: vec![0, 1, 2, 0, 2, 3],
        }
    }

    /// A simple cube with per-face colours.
    pub fn cube() -> Self {
        let h = 0.5;
        //      front         back          right        left         top          bottom
        let verts = vec![
            Vertex { position: [-h, -h,  h], color: [1.0, 0.0, 0.0, 1.0] },
            Vertex { position: [ h, -h,  h], color: [1.0, 0.0, 0.0, 1.0] },
            Vertex { position: [ h,  h,  h], color: [1.0, 0.0, 0.0, 1.0] },
            Vertex { position: [-h,  h,  h], color: [1.0, 0.0, 0.0, 1.0] },
            Vertex { position: [ h, -h, -h], color: [0.0, 1.0, 0.0, 1.0] },
            Vertex { position: [-h, -h, -h], color: [0.0, 1.0, 0.0, 1.0] },
            Vertex { position: [-h,  h, -h], color: [0.0, 1.0, 0.0, 1.0] },
            Vertex { position: [ h,  h, -h], color: [0.0, 1.0, 0.0, 1.0] },
            Vertex { position: [ h, -h,  h], color: [0.0, 0.0, 1.0, 1.0] },
            Vertex { position: [ h, -h, -h], color: [0.0, 0.0, 1.0, 1.0] },
            Vertex { position: [ h,  h, -h], color: [0.0, 0.0, 1.0, 1.0] },
            Vertex { position: [ h,  h,  h], color: [0.0, 0.0, 1.0, 1.0] },
            Vertex { position: [-h, -h, -h], color: [1.0, 1.0, 0.0, 1.0] },
            Vertex { position: [-h, -h,  h], color: [1.0, 1.0, 0.0, 1.0] },
            Vertex { position: [-h,  h,  h], color: [1.0, 1.0, 0.0, 1.0] },
            Vertex { position: [-h,  h, -h], color: [1.0, 1.0, 0.0, 1.0] },
            Vertex { position: [-h,  h,  h], color: [0.0, 1.0, 1.0, 1.0] },
            Vertex { position: [ h,  h,  h], color: [0.0, 1.0, 1.0, 1.0] },
            Vertex { position: [ h,  h, -h], color: [0.0, 1.0, 1.0, 1.0] },
            Vertex { position: [-h,  h, -h], color: [0.0, 1.0, 1.0, 1.0] },
            Vertex { position: [-h, -h, -h], color: [1.0, 0.0, 1.0, 1.0] },
            Vertex { position: [ h, -h, -h], color: [1.0, 0.0, 1.0, 1.0] },
            Vertex { position: [ h, -h,  h], color: [1.0, 0.0, 1.0, 1.0] },
            Vertex { position: [-h, -h,  h], color: [1.0, 0.0, 1.0, 1.0] },
        ];
        let idx: Vec<u16> = vec![
            0, 1, 2, 0, 2, 3,     // front
            4, 5, 6, 4, 6, 7,     // back
            8, 9, 10, 8, 10, 11,  // right
            12, 13, 14, 12, 14, 15, // left
            16, 17, 18, 16, 18, 19, // top
            20, 21, 22, 20, 22, 23, // bottom
        ];
        Self { vertices: verts, indices: idx }
    }
}