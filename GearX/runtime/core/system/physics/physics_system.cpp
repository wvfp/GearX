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
				rigidbody_com->setDirty(true);
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
					// 处理Rigidbody组件的更新请求
					if (rigidbody_com && rigidbody_com->isDirty()) {
						// 将物理位置和角度转换为游戏世界坐标并同步到Transform
						transform_com->setPosition({
							rigidbody_com->getPosition()[0] * PPM - texture_com->getDstRect()[2] / 2.0f,
							rigidbody_com->getPosition()[1] * PPM - texture_com->getDstRect()[3] / 2.0f
							});
						transform_com->setRotation(360 * rigidbody_com->getAngle() / (2 * M_PI));

						// 清除Rigidbody的脏标记
						rigidbody_com->setDirty(false);

						// 标记Transform为脏，因为物理状态已更新
						transform_com->setDirty(true);
					}

				// 处理Texture组件的更新请求
					if (texture_com && texture_com->isDirty()) {

						// 将Transform的位置同步到Texture的绘制矩形
						texture_com->setDstRectX(transform_com->getPositionX());
						texture_com->setDstRectY(transform_com->getPositionY());

						// 清除Texture的脏标记
						texture_com->setDirty(false);

						// 由于Texture更新可能影响显示位置，标记Transform为脏
						transform_com->setDirty(true);
					}



					// 处理Transform组件的更新，同步到其他组件
					if (transform_com->isDirty()) {
						// 更新Texture组件
						if (texture_com) {
							// 设置纹理绘制位置和旋转
							texture_com->dstRect.x = transform_com->getPosition()[0];
							texture_com->dstRect.y = transform_com->getPosition()[1];
							texture_com->angle = transform_com->getRotation();

							// 处理圆形纹理的特殊情况
							if (texture_com->getShape() == TextureComponent::Shape::Circle) {
								float L = std::max(texture_com->dstRect.w, texture_com->dstRect.h);
								texture_com->dstRect.w = L;
								texture_com->dstRect.h = L;
							}

							// 设置纹理原点为中心
							texture_com->orgin.x = (texture_com->getDstRect()[2] / 2.0f);
							texture_com->orgin.y = (texture_com->getDstRect()[3] / 2.0f);

							// 清除Texture脏标记
							texture_com->setDirty(false);
						}

						// 更新Rigidbody组件,仅在非游戏模式下更新
						if (rigidbody_com && !RuntimeGlobalContext::isGameMode) {
							// 根据是否有纹理组件调整物理位置计算方式
							rigidbody_com->setPosition({
								(transform_com->getPosition()[0] + texture_com->getDstRect()[2] / 2.0f) / PPM,
								(transform_com->getPosition()[1] + texture_com->getDstRect()[3] / 2.0f) / PPM
							});

							// 设置物理角度
							rigidbody_com->setAngle(2 * M_PI * transform_com->getRotation() / 360.0f);

							// 清除Rigidbody脏标记
							rigidbody_com->setDirty(false);
						}

						// 清除Transform脏标记
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
			world.Step(1.0f/RuntimeGlobalContext::DEFAULT_FPS,8, 3);
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
		RuntimeGlobalContext::world.getCurrentLevel()->resetWorld();
		for (auto& obj : objs) {
			auto com2 = obj.second->getComponentByTypeName(rttr::type::get<RigidBodyComponent>().get_name());
			auto rigidbody_com = std::dynamic_pointer_cast<RigidBodyComponent>(com2);
			if (!rigidbody_com)
				continue;
			rigidbody_com->destroy();
		}
	}
}