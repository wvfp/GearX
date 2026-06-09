//! Mesh primitives for the GearX render module.

/// A single vertex: position (vec3) + uv (vec2) + color (vec4).
///
/// GPU layout (stride = 36 bytes):
///   0 — position: vec3f  (offset 0,  12 bytes)
///   1 — uv:       vec2f  (offset 12,  8 bytes)
///   2 — color:    vec4f  (offset 20, 16 bytes)
#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub struct Vertex {
    pub position: [f32; 3],
    pub uv: [f32; 2],
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
    pub fn triangle() -> Self {
        Self {
            vertices: vec![
                Vertex { position: [0.0, 0.5, 0.0],  uv: [0.5, 0.0], color: [1.0, 0.0, 0.0, 1.0] },
                Vertex { position: [-0.5, -0.5, 0.0], uv: [0.0, 1.0], color: [0.0, 1.0, 0.0, 1.0] },
                Vertex { position: [0.5, -0.5, 0.0],  uv: [1.0, 1.0], color: [0.0, 0.0, 1.0, 1.0] },
            ],
            indices: vec![0, 1, 2],
        }
    }

    pub fn quad() -> Self {
        let h: f32 = 0.5;
        Self {
            vertices: vec![
                Vertex { position: [-h, -h, 0.0], uv: [0.0, 1.0], color: [1.0; 4] },
                Vertex { position: [ h, -h, 0.0], uv: [1.0, 1.0], color: [1.0; 4] },
                Vertex { position: [ h,  h, 0.0], uv: [1.0, 0.0], color: [1.0; 4] },
                Vertex { position: [-h,  h, 0.0], uv: [0.0, 0.0], color: [1.0; 4] },
            ],
            indices: vec![0, 1, 2, 0, 2, 3],
        }
    }

    pub fn cube() -> Self {
        let h = 0.5;
        let v = |p: [f32; 3], c: [f32; 4]| Vertex { position: p, uv: [0.0, 0.0], color: c };
        let verts = vec![
            v([-h, -h,  h], [1.0, 0.0, 0.0, 1.0]), v([ h, -h,  h], [1.0, 0.0, 0.0, 1.0]),
            v([ h,  h,  h], [1.0, 0.0, 0.0, 1.0]), v([-h,  h,  h], [1.0, 0.0, 0.0, 1.0]),
            v([ h, -h, -h], [0.0, 1.0, 0.0, 1.0]), v([-h, -h, -h], [0.0, 1.0, 0.0, 1.0]),
            v([-h,  h, -h], [0.0, 1.0, 0.0, 1.0]), v([ h,  h, -h], [0.0, 1.0, 0.0, 1.0]),
            v([ h, -h,  h], [0.0, 0.0, 1.0, 1.0]), v([ h, -h, -h], [0.0, 0.0, 1.0, 1.0]),
            v([ h,  h, -h], [0.0, 0.0, 1.0, 1.0]), v([ h,  h,  h], [0.0, 0.0, 1.0, 1.0]),
            v([-h, -h, -h], [1.0, 1.0, 0.0, 1.0]), v([-h, -h,  h], [1.0, 1.0, 0.0, 1.0]),
            v([-h,  h,  h], [1.0, 1.0, 0.0, 1.0]), v([-h,  h, -h], [1.0, 1.0, 0.0, 1.0]),
            v([-h,  h,  h], [0.0, 1.0, 1.0, 1.0]), v([ h,  h,  h], [0.0, 1.0, 1.0, 1.0]),
            v([ h,  h, -h], [0.0, 1.0, 1.0, 1.0]), v([-h,  h, -h], [0.0, 1.0, 1.0, 1.0]),
            v([-h, -h, -h], [1.0, 0.0, 1.0, 1.0]), v([ h, -h, -h], [1.0, 0.0, 1.0, 1.0]),
            v([ h, -h,  h], [1.0, 0.0, 1.0, 1.0]), v([-h, -h,  h], [1.0, 0.0, 1.0, 1.0]),
        ];
        let idx: Vec<u16> = vec![
            0, 1, 2, 0, 2, 3,
            4, 5, 6, 4, 6, 7,
            8, 9, 10, 8, 10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23,
        ];
        Self { vertices: verts, indices: idx }
    }
}
