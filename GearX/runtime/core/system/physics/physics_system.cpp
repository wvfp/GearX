#include "physics_system.hpp"
#include "../../global/global.hpp"
#include "../../framework/component/rigidbody/rigidbody_component.hpp"
#include "../../framework/component/transform/transform_component.hpp"
#include "../../framework/component/texture/texture_component.hpp"
namespace GearX {
	static void UpdateRigidBodyTransformWithThreadPool(GearX::LevelObjectMap& objs) {
		// 提交所有对象的处理任务
		std::vector<std::future<void>> futures;
		futures.reserve(objs.size());

		for (auto& obj : objs) {
			futures.emplace_back(RuntimeGlobalContext::threadPool->enqueue([&obj] {
				std::array<float, 2> scale = { 1.0f,1.0f };
				auto com2 = obj.second->getComponentByTypeName(rttr::type::get<RigidBodyComponent>().get_name());
				auto rigidbody_com = std::dynamic_pointer_cast<RigidBodyComponent>(com2);
				if (!rigidbody_com)
					return;
				rigidbody_com->updateTransform();
				}));
		}

		// 等待所有任务完成
		RuntimeGlobalContext::threadPool->waitForCompletion();
	}
	static void UpdateTransformWithThreadPool(GearX::LevelObjectMap& objs) {
		// 提交所有对象的处理任务
		std::vector<std::future<void>> futures;
		futures.reserve(objs.size());

		for (auto& obj : objs) {
			futures.emplace_back(RuntimeGlobalContext::threadPool->enqueue([&obj] {
				auto com = obj.second->getComponentByTypeName(rttr::type::get<TransformComponent>().get_name());
				auto com2 = obj.second->getComponentByTypeName(rttr::type::get<RigidBodyComponent>().get_name());
				auto com3 = obj.second->getComponentByTypeName(rttr::type::get<TextureComponent>().get_name());
				auto transform_com = std::dynamic_pointer_cast<TransformComponent>(com);
				auto rigidbody_com = std::dynamic_pointer_cast<RigidBodyComponent>(com2);
				auto texture_com = std::dynamic_pointer_cast<TextureComponent>(com3);
				if (com) {
					if (texture_com && texture_com->isDirty()) {
						transform_com->setPosition({ texture_com->getDstRect()[0],
							texture_com->getDstRect()[1] });
						transform_com->setRotation(texture_com->angle);
					}
					if (rigidbody_com && rigidbody_com->isDirty() && !transform_com->isDirty()) {
						transform_com->setPosition({ rigidbody_com->getPosition()[0] * PPM - texture_com->getDstRect()[2] / 2.0f,
							rigidbody_com->getPosition()[1] * PPM - texture_com->getDstRect()[3] / 2.0f });
						transform_com->setRotation(360 * rigidbody_com->getAngle() / (2 * M_PI));
						rigidbody_com->setDirty(false);
					}
					else if (rigidbody_com && transform_com->isDirty() && rigidbody_com->isDirty()) {
							
					}
					if (transform_com->isDirty()) {
						if (texture_com) {
							texture_com->dstRect.x = transform_com->getPosition()[0];
							texture_com->dstRect.y = transform_com->getPosition()[1];
							texture_com->angle = transform_com->getRotation();
							if (texture_com->getShape() == TextureComponent::Shape::Circle) {
								float L = std::max(texture_com->getDstRect()[2], texture_com->getDstRect()[3]);
								texture_com->setDstRect({ texture_com->dstRect.x,texture_com->dstRect.y,L,L });
							}
							texture_com->orgin.x = (texture_com->getDstRect()[2] / 2.0f);
							texture_com->orgin.y = (texture_com->getDstRect()[3] / 2.0f);
							texture_com->setDirty(false);
						}
						if (rigidbody_com) {
							if (nullptr == texture_com) {
								rigidbody_com->setPosition({ (transform_com->getPosition()[0]) / PPM,(transform_com->getPosition()[1]) / PPM });
							}
							else {
								rigidbody_com->setPosition({ (transform_com->getPosition()[0] + texture_com->getDstRect()[2] / 2.0f) / PPM,
									(transform_com->getPosition()[1] + texture_com->getDstRect()[3] / 2.0f) / PPM });
							}
							rigidbody_com->setAngle(2 * M_PI * transform_com->getRotation() / 360.0f);
							rigidbody_com->setDirty(false);
						}
						transform_com->setDirty(false);
					}
				}
				}));
		}

		// 等待所有任务完成
		RuntimeGlobalContext::threadPool->waitForCompletion();
	}
}
void GearX::PhysicsSystem::tick(float deltaTime) {
	if (!RuntimeGlobalContext::world.getCurrentLevel())
		return;
	else {
		if (isPhysicsInit == false) {
			auto& objs = RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
			for (auto& obj : objs) {
				auto com2 = obj.second->getComponentByTypeName(rttr::type::get<RigidBodyComponent>().get_name());
				auto rigidbody_com = std::dynamic_pointer_cast<RigidBodyComponent>(com2);
				if (!rigidbody_com)
					continue;
				rigidbody_com->init();
			}
			isPhysicsInit = true;
		}

		if (isPhysicsInit) {
			b2World& world = RuntimeGlobalContext::world.getCurrentLevel()->getWorld();
			world.Step(deltaTime, 8, 3);
			auto& objs = RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
			// 使用线程池处理所有对象的更新
			UpdateRigidBodyTransformWithThreadPool(objs);
		}
	}
}

void GearX::PhysicsSystem::updateTransform() {
	if (!RuntimeGlobalContext::world.getCurrentLevel())
		return;
	else {
		auto& objs = RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
		//线程池处理所有对象的更新
		UpdateTransformWithThreadPool(objs);
	}
}

void GearX::PhysicsSystem::destroy() {
	if (isPhysicsInit) {
		isPhysicsInit = false;
		auto& objs = RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
		RuntimeGlobalContext::world.getCurrentLevel()->getWorld().ClearForces();
		for (auto& obj : objs) {
			auto com2 = obj.second->getComponentByTypeName(rttr::type::get<RigidBodyComponent>().get_name());
			auto rigidbody_com = std::dynamic_pointer_cast<RigidBodyComponent>(com2);
			if (!rigidbody_com)
				continue;
			RuntimeGlobalContext::world.getCurrentLevel()->getWorld().DestroyBody(rigidbody_com->getBody());
			rigidbody_com->destroy();
		}
	}
}