//! Mesh loading and primitive factories.

/// A single vertex matching the render module's GPU layout.
#[derive(Debug, Clone, Copy)]
#[repr(C)]
pub struct Vertex {
    pub position: [f32; 3],
    pub uv: [f32; 2],
    pub color: [f32; 4],
}

unsafe impl bytemuck::Pod for Vertex {}
unsafe impl bytemuck::Zeroable for Vertex {}

/// A mesh loaded into CPU memory, ready for GPU upload.
#[derive(Debug, Clone)]
pub struct LoadedMesh {
    pub vertices: Vec<Vertex>,
    pub indices: Vec<u16>,
    pub name: String,
}

impl LoadedMesh {
    fn new(name: &str, vertices: Vec<Vertex>, indices: Vec<u16>) -> Self {
        Self { vertices, indices, name: name.to_owned() }
    }

    pub fn triangle() -> Self {
        Self::new(
            "triangle",
            vec![
                Vertex { position: [0.0, 0.5, 0.0],  uv: [0.5, 0.0], color: [1.0, 0.0, 0.0, 1.0] },
                Vertex { position: [-0.5, -0.5, 0.0], uv: [0.0, 1.0], color: [0.0, 1.0, 0.0, 1.0] },
                Vertex { position: [0.5, -0.5, 0.0],  uv: [1.0, 1.0], color: [0.0, 0.0, 1.0, 1.0] },
            ],
            vec![0, 1, 2],
        )
    }

    pub fn quad() -> Self {
        let h = 0.5;
        Self::new(
            "quad",
            vec![
                Vertex { position: [-h, -h, 0.0], uv: [0.0, 1.0], color: [1.0; 4] },
                Vertex { position: [ h, -h, 0.0], uv: [1.0, 1.0], color: [1.0; 4] },
                Vertex { position: [ h,  h, 0.0], uv: [1.0, 0.0], color: [1.0; 4] },
                Vertex { position: [-h,  h, 0.0], uv: [0.0, 0.0], color: [1.0; 4] },
            ],
            vec![0, 1, 2, 0, 2, 3],
        )
    }

    pub fn cube() -> Self {
        let h = 0.5;
        let v = |p: [f32; 3], c: [f32; 4]| Vertex { position: p, uv: [0.0, 0.0], color: c };
        let verts: Vec<Vertex> = vec![
            v([-h,-h, h],[1.0,0.0,0.0,1.0]), v([ h,-h, h],[1.0,0.0,0.0,1.0]),
            v([ h, h, h],[1.0,0.0,0.0,1.0]), v([-h, h, h],[1.0,0.0,0.0,1.0]),
            v([ h,-h,-h],[0.0,1.0,0.0,1.0]), v([-h,-h,-h],[0.0,1.0,0.0,1.0]),
            v([-h, h,-h],[0.0,1.0,0.0,1.0]), v([ h, h,-h],[0.0,1.0,0.0,1.0]),
            v([ h,-h, h],[0.0,0.0,1.0,1.0]), v([ h,-h,-h],[0.0,0.0,1.0,1.0]),
            v([ h, h,-h],[0.0,0.0,1.0,1.0]), v([ h, h, h],[0.0,0.0,1.0,1.0]),
            v([-h,-h,-h],[1.0,1.0,0.0,1.0]), v([-h,-h, h],[1.0,1.0,0.0,1.0]),
            v([-h, h, h],[1.0,1.0,0.0,1.0]), v([-h, h,-h],[1.0,1.0,0.0,1.0]),
            v([-h, h, h],[0.0,1.0,1.0,1.0]), v([ h, h, h],[0.0,1.0,1.0,1.0]),
            v([ h, h,-h],[0.0,1.0,1.0,1.0]), v([-h, h,-h],[0.0,1.0,1.0,1.0]),
            v([-h,-h,-h],[1.0,0.0,1.0,1.0]), v([ h,-h,-h],[1.0,0.0,1.0,1.0]),
            v([ h,-h, h],[1.0,0.0,1.0,1.0]), v([-h,-h, h],[1.0,0.0,1.0,1.0]),
        ];
        let idx = vec![
            0,1,2, 0,2,3, 4,5,6, 4,6,7, 8,9,10, 8,10,11,
            12,13,14, 12,14,15, 16,17,18, 16,18,19, 20,21,22, 20,22,23,
        ];
        Self::new("cube", verts, idx)
    }
}

/// Loads mesh data from disk (simple OBJ format support).
pub struct MeshLoader;

impl MeshLoader {
    pub fn new() -> Self { Self }

    pub fn load_from_obj(path: &std::path::Path) -> anyhow::Result<LoadedMesh> {
        let source = std::fs::read_to_string(path)?;
        let mut positions: Vec<[f32; 3]> = Vec::new();
        let mut vertices: Vec<Vertex> = Vec::new();
        let mut indices: Vec<u16> = Vec::new();

        for line in source.lines() {
            let line = line.trim();
            if line.starts_with("v ") {
                let parts: Vec<f32> = line
                    .split_whitespace()
                    .skip(1)
                    .filter_map(|s| s.parse().ok())
                    .collect();
                if parts.len() >= 3 {
                    positions.push([parts[0], parts[1], parts[2]]);
                }
            } else if line.starts_with("f ") {
                let parts: Vec<&str> = line.split_whitespace().skip(1).collect();
                if parts.len() >= 3 {
                    let tri: Vec<u16> = parts
                        .iter()
                        .filter_map(|p| {
                            let idx_str = p.split('/').next().unwrap_or(p);
                            idx_str.parse::<u16>().ok().map(|i| i.wrapping_sub(1))
                        })
                        .collect();
                    for i in 1..tri.len().saturating_sub(1) {
                        indices.push(tri[0]);
                        indices.push(tri[i]);
                        indices.push(tri[i + 1]);
                    }
                }
            }
        }

        for pos in &positions {
            vertices.push(Vertex {
                position: *pos,
                uv: [0.0, 0.0],
                color: [0.8, 0.8, 0.8, 1.0],
            });
        }

        let name = path
            .file_stem()
            .and_then(|s| s.to_str())
            .unwrap_or("unknown")
            .to_owned();
        Ok(LoadedMesh { vertices, indices, name })
    }
}
