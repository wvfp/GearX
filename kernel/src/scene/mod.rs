pub mod transform;
pub mod loader;

use bevy_ecs::component::Component;
use serde::{Deserialize, Serialize};

pub use transform::Transform;

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct Scene {
    pub version: u32,
    pub entities: Vec<EntityDef>,
    #[serde(default)]
    pub resources: Vec<ResourceDef>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct EntityDef {
    pub id: Option<u64>,
    pub name: String,
    pub parent: Option<u64>,
    #[serde(default)]
    pub components: Vec<ComponentDef>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ComponentDef {
    pub type_name: String,
    pub data: serde_json::Value,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
pub struct ResourceDef {
    pub type_name: String,
    pub data: serde_json::Value,
}

/// Names an entity for display and serialization.
#[derive(Component, Debug, Clone, Serialize, Deserialize)]
pub struct Name(pub String);
