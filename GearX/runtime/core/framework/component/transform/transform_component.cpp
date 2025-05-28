#include "transform_component.hpp"

namespace GearX {
	// RTTR Register TransformComponent
	RTTR_REGISTRATION
	{
		rttr::registration::class_<TransformComponent>("TransformComponent")
		.constructor<>()
		.property(u8"位置", &TransformComponent::getPosition, &TransformComponent::setPosition)
		.property(u8"缩放",&TransformComponent::getScale,&TransformComponent::setScale)
		.property(u8"旋转中心", &TransformComponent::getOrigin, &TransformComponent::setOrigin)
		.property(u8"旋转角度", &TransformComponent::getRotation, &TransformComponent::setRotation)
		.method("addComponentTo",&TransformComponent::addComponentTo);
	};
	TransformComponent::TransformComponent() {
		this->m_type_name = "TransformComponent";
	};
	void TransformComponent::addComponentTo(std::shared_ptr<GObject> obj) {
		if (obj) {
			auto com = std::make_shared<TransformComponent>();
			obj->addComponent(com);
		}
	}
	std::shared_ptr<Component> GearX::TransformComponent::copy(){
		auto com = std::make_shared<TransformComponent>();
		*com = *this;
		com->m_parentObject.reset();
	    return com;
	}
	TransformComponent::~TransformComponent() = default;
	void TransformComponent::setOriginX(float x) {
		originX = x;
		m_isDirty = true;
	}
	void TransformComponent::setOriginY(float y) {
		originY = y;
		m_isDirty = true;
	}
	void TransformComponent::setPositionY(float y) {
		this->y = y;
		m_isDirty = true;
	}
	void TransformComponent::setPositionX(float x) {
		this->x = x;
		m_isDirty = true;
	}

	void TransformComponent::setOrigin(std::array<float, 2> o) {
		originX = o[0];
		originY = o[1];
		m_isDirty = true;
	}

	void TransformComponent::setPosition(std::array<float, 2> p) {
		this->x = p[0];
		this->y = p[1];
		m_isDirty = true;
	}

	void TransformComponent::setScale(std::array<float, 2> s) {
		if (s[0] < 0)
			s[0] = 0;
		if (s[1] < 0)
			s[1] = 0;
		scaleX = s[0];
		scaleY = s[1];
		m_isDirty = true;
	}

	void TransformComponent::setScaleX(float scale_X) {
		scaleX = scale_X < 0.0f ? 0.0f : scale_X;
		m_isDirty = true;
	}
	void TransformComponent::setScaleY(float scale_Y) {
		scaleY = scale_Y < 0.0f ? 0.0f : scale_Y;
		m_isDirty = true;
	}
	void TransformComponent::setRotation(float angle) {
		if (angle > 360.0f) {
			angle = angle - 360.0f;
		}
		if (angle < 0) {
			angle = angle + 360.0f;
		}
		rotation = angle;
		m_isDirty = true;
	}

	std::array<float, 2> TransformComponent::getPosition() const {
		return { x, y };
	}

	float TransformComponent::getPositionX() const {
		return x;
	}

	float TransformComponent::getPositionY() const {
		return y;
	}

	std::array<float, 2> TransformComponent::getScale() const {
		return { scaleX, scaleY };
	}
	float TransformComponent::getScaleX() const {
		return scaleX;
	}
	float TransformComponent::getScaleY() const {
		return scaleY;
	}
	float TransformComponent::getOriginX() const {
		return originX;
	}

	float TransformComponent::getOriginY() const {
		return originY;
	}

	float TransformComponent::getRotation() const {
		return rotation;
	}
	std::array<float, 2> TransformComponent::getOrigin() const {
		return { originX, originY };
	}

	template<typename Archive>
	void TransformComponent::serialize(Archive& archive) {
		Component::serialize(archive);
		archive(cereal::make_nvp("x", x),
			cereal::make_nvp("y", y),
			cereal::make_nvp("rotation", rotation),
			cereal::make_nvp("scaleX", scaleX),
			cereal::make_nvp("scaleY", scaleY),
			cereal::make_nvp("originX", originX),
			cereal::make_nvp("originY", originY));
	}
}
using namespace GearX;
CEREAL_REGISTER_DYNAMIC_INIT(TransformComponent)