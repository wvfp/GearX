use std::collections::HashMap;

use anyhow::{anyhow, Result};
use bevy_ecs::entity::Entity;
use bevy_ecs::world::{EntityWorldMut, World};

use super::{ComponentDef, EntityDef, Name, Scene, Transform};

type DeserializeFn = Box<dyn Fn(&serde_json::Value, &mut EntityWorldMut) -> Result<()> + Send + Sync>;
type SerializeFn = Box<dyn Fn(Entity, &World) -> Result<Option<serde_json::Value>> + Send + Sync>;

/// Registry mapping component type names to (de)serialization functions.
pub struct ComponentRegistry {
    deserializers: HashMap<String, DeserializeFn>,
    serializers: HashMap<String, SerializeFn>,
}

impl bevy_ecs::system::Resource for ComponentRegistry {}

impl ComponentRegistry {
    pub fn new() -> Self {
        let mut reg = Self {
            deserializers: HashMap::new(),
            serializers: HashMap::new(),
        };
        reg.register_defaults();
        reg
    }

    fn register_defaults(&mut self) {
        self.register::<Transform>(
            "Transform",
            |data, entity| {
                let t: Transform = serde_json::from_value(data.clone())?;
                entity.insert(t);
                Ok(())
            },
            |entity, world| {
                if let Some(t) = world.get::<Transform>(entity) {
                    Ok(Some(serde_json::to_value(t)?))
                } else {
                    Ok(None)
                }
            },
        );
        self.register::<Name>(
            "Name",
            |data, entity| {
                let n: Name = serde_json::from_value(data.clone())?;
                entity.insert(n);
                Ok(())
            },
            |entity, world| {
                if let Some(n) = world.get::<Name>(entity) {
                    Ok(Some(serde_json::to_value(n)?))
                } else {
                    Ok(None)
                }
            },
        );
    }

    pub fn register<T: Send + Sync + 'static>(
        &mut self,
        type_name: &str,
        deserialize: impl Fn(&serde_json::Value, &mut EntityWorldMut) -> Result<()> + Send + Sync + 'static,
        serialize: impl Fn(Entity, &World) -> Result<Option<serde_json::Value>> + Send + Sync + 'static,
    ) {
        self.deserializers.insert(type_name.to_owned(), Box::new(deserialize));
        self.serializers.insert(type_name.to_owned(), Box::new(serialize));
    }

    pub fn known_types(&self) -> Vec<&str> {
        self.deserializers.keys().map(|s| s.as_str()).collect()
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// SceneLoader
// ═══════════════════════════════════════════════════════════════════════════

pub struct SceneLoader;

impl SceneLoader {
    pub fn load(path: &std::path::Path, world: &mut World) -> Result<Vec<Entity>> {
        let json = std::fs::read_to_string(path)?;
        Self::load_from_str(&json, world)
    }

    pub fn load_from_str(json: &str, world: &mut World) -> Result<Vec<Entity>> {
        let scene: Scene = serde_json::from_str(json)?;
        Self::spawn_scene(&scene, world)
    }

    fn spawn_scene(scene: &Scene, world: &mut World) -> Result<Vec<Entity>> {
        let registry = world
            .remove_resource::<ComponentRegistry>()
            .unwrap_or_else(ComponentRegistry::new);

        let mut entities = Vec::with_capacity(scene.entities.len());

        for entity_def in &scene.entities {
            let mut entity = world.spawn(Name(entity_def.name.clone()));
            // Insert Name as a component too (spawn already added it above).
            for comp in &entity_def.components {
                if comp.type_name == "Name" {
                    continue; // already added via spawn
                }
                if let Some(deser) = registry.deserializers.get(&comp.type_name) {
                    deser(&comp.data, &mut entity)?;
                } else {
                    tracing::warn!("Unknown component type '{}' — skipped", comp.type_name);
                }
            }
            entities.push(entity.id());
        }

        world.insert_resource(registry);
        Ok(entities)
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// SceneSaver
// ═══════════════════════════════════════════════════════════════════════════

pub struct SceneSaver;

impl SceneSaver {
    pub fn save(entities: &[Entity], world: &World, path: &std::path::Path) -> Result<()> {
        let json = Self::to_string(entities, world)?;
        std::fs::write(path, json)?;
        Ok(())
    }

    pub fn to_string(entities: &[Entity], world: &World) -> Result<String> {
        let scene = Self::collect_scene(entities, world)?;
        Ok(serde_json::to_string_pretty(&scene)?)
    }

    fn collect_scene(entities: &[Entity], world: &World) -> Result<Scene> {
        let registry = world
            .get_resource::<ComponentRegistry>()
            .ok_or_else(|| anyhow!("ComponentRegistry not found in world"))?;

        let mut entity_defs = Vec::with_capacity(entities.len());
        for (i, &entity) in entities.iter().enumerate() {
            let name = world
                .get::<Name>(entity)
                .map(|n| n.0.clone())
                .unwrap_or_else(|| format!("Entity_{}", i));

            let mut components = Vec::new();
            for (type_name, serializer) in &registry.serializers {
                if let Some(data) = serializer(entity, world)? {
                    components.push(ComponentDef {
                        type_name: type_name.clone(),
                        data,
                    });
                }
            }

            entity_defs.push(EntityDef {
                id: Some(i as u64 + 1),
                name,
                parent: None,
                components,
            });
        }

        Ok(Scene {
            version: 1,
            entities: entity_defs,
            resources: Vec::new(),
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn scene_round_trip() {
        let scene = Scene {
            version: 1,
            entities: vec![EntityDef {
                id: Some(1),
                name: "TestEntity".into(),
                parent: None,
                components: vec![ComponentDef {
                    type_name: "Transform".into(),
                    data: serde_json::json!({
                        "position": [1.0, 2.0, 3.0],
                        "rotation": [0.0, 0.0, 0.0, 1.0],
                        "scale": [1.0, 1.0, 1.0],
                    }),
                }],
            }],
            resources: Vec::new(),
        };
        let json = serde_json::to_string(&scene).unwrap();
        let parsed: Scene = serde_json::from_str(&json).unwrap();
        assert_eq!(parsed.version, 1);
        assert_eq!(parsed.entities.len(), 1);
        assert_eq!(parsed.entities[0].name, "TestEntity");
    }

    #[test]
    fn load_scene_spawns_entities() {
        let json = r#"{
            "version": 1,
            "entities": [
                {
                    "name": "A",
                    "components": [
                        { "type_name": "Transform", "data": { "position": [1.0, 0.0, 0.0], "rotation": [0,0,0,1], "scale": [1,1,1] } }
                    ]
                },
                {
                    "name": "B",
                    "components": []
                }
            ]
        }"#;
        let mut world = World::new();
        let entities = SceneLoader::load_from_str(json, &mut world).unwrap();
        assert_eq!(entities.len(), 2);
        assert!(world.get::<Transform>(entities[0]).is_some());
        assert_eq!(world.get::<Transform>(entities[0]).unwrap().position, [1.0, 0.0, 0.0]);
        assert!(world.get::<Name>(entities[0]).is_some());
        assert_eq!(world.get::<Name>(entities[0]).unwrap().0, "A");
    }

    #[test]
    fn save_scene_produces_valid_json() {
        let mut world = World::new();
        world.insert_resource(ComponentRegistry::new());
        let e = world.spawn((Name("Player".into()), Transform::from_position(0.0, 1.0, 0.0))).id();
        let json = SceneSaver::to_string(&[e], &world).unwrap();
        let scene: Scene = serde_json::from_str(&json).unwrap();
        assert_eq!(scene.entities.len(), 1);
        assert_eq!(scene.entities[0].name, "Player");
    }

    #[test]
    fn unknown_component_type_is_skipped() {
        let json = r#"{
            "version": 1,
            "entities": [{
                "name": "X",
                "components": [{ "type_name": "Physics", "data": {} }]
            }]
        }"#;
        let mut world = World::new();
        let entities = SceneLoader::load_from_str(json, &mut world).unwrap();
        assert_eq!(entities.len(), 1);
    }
}
