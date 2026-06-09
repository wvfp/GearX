use bevy_ecs::component::Component;
use serde::{Deserialize, Serialize};

#[derive(Component, Debug, Clone, Serialize, Deserialize)]
pub struct Transform {
    pub position: [f32; 3],
    pub rotation: [f32; 4],
    pub scale: [f32; 3],
}

impl Default for Transform {
    fn default() -> Self {
        Self {
            position: [0.0; 3],
            rotation: [0.0, 0.0, 0.0, 1.0],
            scale: [1.0; 3],
        }
    }
}

impl Transform {
    pub fn from_position(x: f32, y: f32, z: f32) -> Self {
        Self { position: [x, y, z], ..Default::default() }
    }

    pub fn translation(&self) -> glam::Vec3 {
        glam::Vec3::from(self.position)
    }

    pub fn rotation_quat(&self) -> glam::Quat {
        glam::Quat::from_array(self.rotation)
    }

    pub fn scale_vec(&self) -> glam::Vec3 {
        glam::Vec3::from(self.scale)
    }
}
