#include "physics_system.hpp"
#include "../../global/global.hpp"
#include "../../framework/component/rigidbody/rigidbody_component.hpp"
#include "../../framework/component/transform/transform_component.hpp"
#include "../../framework/component/texture/texture_component.hpp"
namespace GearX {
	static void UpdateRigidBodyTransformWithThreadPool(GearX::LevelObjectMap& objs) {
		// �ύ���ж���Ĵ�������
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

		// �ȴ������������
		RuntimeGlobalContext::threadPool->waitForCompletion();
	}
	static void UpdateTransformWithThreadPool(GearX::LevelObjectMap& objs) {
		// �ύ���ж���Ĵ�������
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
					// ����Rigidbody����ĸ�������
					if (rigidbody_com && rigidbody_com->isDirty()) {
						// ������λ�úͽǶ�ת��Ϊ��Ϸ�������겢ͬ����Transform
						transform_com->setPosition({
							rigidbody_com->getPosition()[0] * PPM - texture_com->getDstRect()[2] / 2.0f,
							rigidbody_com->getPosition()[1] * PPM - texture_com->getDstRect()[3] / 2.0f
							});
						transform_com->setRotation(360 * rigidbody_com->getAngle() / (2 * M_PI));

						// ���Rigidbody������
						rigidbody_com->setDirty(false);

						// ���TransformΪ�࣬��Ϊ����״̬�Ѹ���
						transform_com->setDirty(true);
					}

				// ����Texture����ĸ�������
					if (texture_com && texture_com->isDirty()) {

						// ��Transform��λ��ͬ����Texture�Ļ��ƾ���
						texture_com->setDstRectX(transform_com->getPositionX());
						texture_com->setDstRectY(transform_com->getPositionY());

						// ���Texture������
						texture_com->setDirty(false);

						// ����Texture���¿���Ӱ����ʾλ�ã����TransformΪ��
						transform_com->setDirty(true);
					}



					// ����Transform����ĸ��£�ͬ�����������
					if (transform_com->isDirty()) {
						// ����Texture���
						if (texture_com) {
							// �����������λ�ú���ת
							texture_com->dstRect.x = transform_com->getPosition()[0];
							texture_com->dstRect.y = transform_com->getPosition()[1];
							texture_com->angle = transform_com->getRotation();

							// ����Բ��������������
							if (texture_com->getShape() == TextureComponent::Shape::Circle) {
								float L = std::max(texture_com->dstRect.w, texture_com->dstRect.h);
								texture_com->dstRect.w = L;
								texture_com->dstRect.h = L;
							}

							// ��������ԭ��Ϊ����
							texture_com->orgin.x = (texture_com->getDstRect()[2] / 2.0f);
							texture_com->orgin.y = (texture_com->getDstRect()[3] / 2.0f);

							// ���Texture����
							texture_com->setDirty(false);
						}

						// ����Rigidbody���,���ڷ���Ϸģʽ�¸���
						if (rigidbody_com && !RuntimeGlobalContext::isGameMode) {
							// �����Ƿ������������������λ�ü��㷽ʽ
							rigidbody_com->setPosition({
								(transform_com->getPosition()[0] + texture_com->getDstRect()[2] / 2.0f) / PPM,
								(transform_com->getPosition()[1] + texture_com->getDstRect()[3] / 2.0f) / PPM
							});

							// ��������Ƕ�
							rigidbody_com->setAngle(2 * M_PI * transform_com->getRotation() / 360.0f);

							// ���Rigidbody����
							rigidbody_com->setDirty(false);
						}

						// ���Transform����
						transform_com->setDirty(false);
					}
					
				}
				}));
		}

		// �ȴ������������
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
			// ʹ���̳߳ش������ж���ĸ���
			UpdateRigidBodyTransformWithThreadPool(objs);
		}
	}
}

void GearX::PhysicsSystem::updateTransform() {
	if (!RuntimeGlobalContext::world.getCurrentLevel())
		return;
	else {
		auto& objs = RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
		//�̳߳ش������ж���ĸ���
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