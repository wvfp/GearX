#include "rigidbody_component.hpp"
#include "../../../utils/points_of_texture.hpp"
#include "../texture/texture_component.hpp"
#include "../transform/transform_component.hpp"

namespace GearX {
	RTTR_REGISTRATION
	{
		// 注册 b2BodyType 枚举
		rttr::registration::enumeration<b2BodyType>(u8"BodyType")
		(
			rttr::value(u8"静态刚体", b2_staticBody),
			rttr::value(u8"运动刚体", b2_kinematicBody),
			rttr::value(u8"动态刚体", b2_dynamicBody)
		);

	// 注册 RigidBodyComponent 类
	rttr::registration::class_<GearX::RigidBodyComponent>("RigidBodyComponent")
		.constructor<>()
		.property(u8"允许睡眠", &RigidBodyComponent::getAlllowSleep, &RigidBodyComponent::setAllowSleep)
		.property(u8"唤醒状态", &RigidBodyComponent::getAwake, &RigidBodyComponent::setAwake)
		.property(u8"连续碰撞", &RigidBodyComponent::getBullet, &RigidBodyComponent::setBullet)
		.property(u8"刚体位置",&RigidBodyComponent::getPosition,&RigidBodyComponent::setPosition)
		.property(u8"角阻尼", &RigidBodyComponent::getAngularDamping, &RigidBodyComponent::setAngularDamping)
		.property(u8"角速度", &RigidBodyComponent::getAngularVelocity, &RigidBodyComponent::setAngularVelocity)
		.property(u8"线阻尼", &RigidBodyComponent::getLinearDamping, &RigidBodyComponent::setLinearDamping)
		.property(u8"线速度", &RigidBodyComponent::getLinearVelocity, &RigidBodyComponent::setLinearVelocity)
		.property(u8"密度", &RigidBodyComponent::getDensity, &RigidBodyComponent::setDensity)
		.property(u8"摩擦系数", &RigidBodyComponent::getFriction, &RigidBodyComponent::setFriction)
		.property(u8"弹性系数", &RigidBodyComponent::getRestitution, &RigidBodyComponent::setRestitution)
		.property(u8"传感器", &RigidBodyComponent::isSensor, &RigidBodyComponent::setSensor)
		.property(u8"弹性阈值", &RigidBodyComponent::getRestitutionThreshold, &RigidBodyComponent::setRestitutionThreshold)
		.property(u8"固定旋转", &RigidBodyComponent::getFixedRotation, &RigidBodyComponent::setFixedRotation)
		.property(u8"重力缩放", &RigidBodyComponent::getGravityScale, &RigidBodyComponent::setGravityScale)
		.property(u8"角度", &RigidBodyComponent::getAngle, &RigidBodyComponent::setAngle)
		.property(u8"刚体类型", &RigidBodyComponent::getBodyType, &RigidBodyComponent::setBodyType)
		.method(u8"getBody", &RigidBodyComponent::getBody)
		.method(u8"getFixtuew", &RigidBodyComponent::getFixture)
		.method(u8"initialize", &RigidBodyComponent::initialize)
		.method(u8"cleanup", &RigidBodyComponent::cleanup)
		.method("addComponentTo", &RigidBodyComponent::addComponentTo);
	};
	float GearX::clamp(float value, float min, float max) {
		// 限制值在范围内，不使用clamp
		if (value < min) {
			value = min;
		}
		else if (value > max) {
			value = max;
		}
		return value;
	}
	void RigidBodyComponent::init() {
		if (!m_body) {
			auto obj = this->getParentObject();
			auto com1 = obj->getComponentByTypeName(rttr::type::get<TransformComponent>().get_name());
			auto transform_com = std::dynamic_pointer_cast<TransformComponent>(com1);
			auto com2 = obj->getComponentByTypeName(rttr::type::get<TextureComponent>().get_name());
			auto texture_com = std::dynamic_pointer_cast<TextureComponent>(com2);
			//几何中心
			this->setPosition({ (texture_com->getDstRect()[0] + texture_com->getDstRect()[2] / 2.0f) / PPM,
				(texture_com->getDstRect()[1] + texture_com->getDstRect()[3] / 2.0f) / PPM });
			this->setAngle(transform_com->getRotation() * (2 * M_PI) / 360.0f);
			if (RuntimeGlobalContext::world.getCurrentLevel()) {
				b2World& world = RuntimeGlobalContext::world.getCurrentLevel()->getWorld();
				m_body = world.CreateBody(&m_bodyDef);
				if (texture_com->getShape() == TextureComponent::Shape::Circle) {
					b2CircleShape circleShape;
					circleShape.m_radius = std::max(texture_com->getDstRect()[2], texture_com->getDstRect()[3]) / (PPM * 2.0f);
					m_fixtureDef.shape = &circleShape;
					m_fixture = m_body->CreateFixture(&m_fixtureDef);
				}
				else if (texture_com->getShape() == TextureComponent::Shape::Rect) {
					b2PolygonShape polygonShape;
					auto asset = texture_com->getTextureAsset();
					SDL_Texture* texture = static_cast<SDL_Texture*>(asset.data);
					auto points = getPointsFromTexture(texture);
					//if(points.empty())
						polygonShape.SetAsBox((texture_com->getDstRect()[2] / PPM) / 2.0f, (texture_com->getDstRect()[3] / PPM) / 2.0f);
					//else
						//polygonShape.Set(points.data(), points.size());
					m_fixtureDef.shape = &polygonShape;
					m_fixture = m_body->CreateFixture(&m_fixtureDef);
				}
			}
		}
	}
	void GearX::RigidBodyComponent::destroy() {
		m_body = nullptr;
		m_fixture = nullptr;
	}
	void GearX::RigidBodyComponent::updateTransform() {
		auto com = getParentObject()->getComponentByTypeName(rttr::type::get<TransformComponent>().get_name());
		auto com2 = getParentObject()->getComponentByTypeName(rttr::type::get<TextureComponent>().get_name());
		if (com && com2) {
			auto transform = std::dynamic_pointer_cast<TransformComponent>(com);
			auto texture_com = std::dynamic_pointer_cast<TextureComponent>(com2);
			if (texture_com)
				transform->setPosition(std::array<float, 2>({ m_body->GetPosition().x * PPM - texture_com->getDstRect()[2] / 2.0f, m_body->GetPosition().y * PPM - texture_com->getDstRect()[3] / 2.0f }));
			else
				transform->setPosition(std::array<float, 2>({ m_body->GetPosition().x * PPM , m_body->GetPosition().y * PPM }));

			transform->setRotation(360 * m_body->GetAngle() / (2 * M_PI));
			m_bodyDef.linearVelocity = m_body->GetLinearVelocity();
			m_body->GetWorldPoint(m_bodyDef.position);
			m_bodyDef.angle = m_body->GetAngle();
			m_bodyDef.angularVelocity = m_body->GetAngularVelocity();
		}
	}
	std::shared_ptr<Component> GearX::RigidBodyComponent::copy() {
		auto com = std::make_shared<RigidBodyComponent>();
		*com = *this;
		com->m_body = nullptr;
		com->m_fixture = nullptr;
		com->m_parentObject.reset();
		return com;
	}
	void RigidBodyComponent::addComponentTo(std::shared_ptr<GObject> obj) {
		if (obj) {
			auto com = std::make_shared<RigidBodyComponent>();
			auto transform_com = obj->getComponentByTypeName(rttr::type::get<TransformComponent>().get_name());
			if (transform_com == nullptr) {
				TransformComponent::addComponentTo(obj);
			}
			auto texture_com = obj->getComponentByTypeName(rttr::type::get<TextureComponent>().get_name());
			if (texture_com == nullptr) {
				TextureComponent::addComponentTo(obj);
			}
			auto com1 = obj->getComponentByTypeName(rttr::type::get<TransformComponent>().get_name());
			auto com2 = obj->getComponentByTypeName(rttr::type::get<TextureComponent>().get_name());
			auto transform = std::dynamic_pointer_cast<TransformComponent>(com1);
			auto texture = std::dynamic_pointer_cast<TextureComponent>(com2);
			obj->addComponent(com);
			com->setPosition({ (transform->getPosition()[0] + texture->getDstRect()[2] / 2.0f) / PPM,
									(transform->getPosition()[1] + texture->getDstRect()[3] / 2.0f) / PPM });
			com->setAngle(2 * M_PI * transform->getRotation() / 360.0f);
			com->setDirty(false);
		}
	}
}