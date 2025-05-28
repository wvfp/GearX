#pragma once
#include "../component.hpp"

namespace GearX {
	class Component;
	class TransformComponent : public Component {
	private:
		float x{ 0.0f }, y{ 0.0f };
		float rotation{ 0 };
		float scaleX{ 1.0f }, scaleY{ 1.0f };
		float originX{ 0.0f }, originY{ 0.0f };
	public:
		TransformComponent();
		~TransformComponent();
		//RTTR ·´Éäµ÷ÓÃ
		static void addComponentTo(std::shared_ptr<GObject> obj);
		std::shared_ptr<Component> copy();
		template<typename Archive>
		void serialize(Archive& archive);
		// Transform properties setters
		void setPositionX(float);
		void setPositionY(float);
		void setOriginX(float);
		void setOriginY(float);
		void setOrigin(std::array<float, 2>);
		void setPosition(std::array<float, 2>);
		void setScale(std::array<float, 2>);
		void setScaleX(float scale_X);
		void setScaleY(float scale_Y);
		void setRotation(float angle);
		// Transform properties getters
		std::array<float, 2> getPosition() const;
		float getPositionX() const;
		float getPositionY() const;
		std::array<float, 2> getScale() const;
		float getOriginX() const;
		float getOriginY() const;
		float getRotation() const;
		float getScaleX() const;
		float getScaleY() const;
		std::array<float, 2> getOrigin() const;
		// RTTR
		RTTR_ENABLE(Component)
			RTTR_REGISTRATION_FRIEND
	};
}
CEREAL_REGISTER_TYPE(GearX::TransformComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(GearX::Component, GearX::TransformComponent)