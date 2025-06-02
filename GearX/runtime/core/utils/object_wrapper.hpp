#pragma once
#include "../../depencies.hpp"
#include "../framework/component/texture/texture_component.hpp"

// 用于GObject的一个包装器，使得GObject包含的Component中的函数易于使用
// 同时，用于注册到Lua中
namespace GearX {
	template<size_t N>
	std::array<float, N> cvtArray(const sol::table& t);
	class TextureComponent;
	class RigidBodyComponent;
	class TransformComponent;
	enum class TextureComponent::Shape : Uint8;
	enum class TextureComponent::FlipMode : Uint8;
	enum class TextureComponent::BlendMode : Uint8;
	class Asset;
	class GObject;
	class Level;
	class World;
	static std::vector<sol::table> TableOfObjects;
	class GObjectWrapper :std::enable_shared_from_this<GObjectWrapper> {
	public:
		explicit GObjectWrapper(std::shared_ptr<GObject>& object);
		static void RegisterEnum(sol::state& lua);
		sol::table& getTable(sol::state& state);
		static sol::table& getEmptyTable(sol::state& state);
		std::shared_ptr<GObjectWrapper> getSharedPtr() {
			return shared_from_this();
		};
	public:
		bool isValid() const;
		bool isValidTexture() const;
		bool isValidRigidBody() const;
		bool isValidTransform() const;
		//For Object
		GObjectID getID();
		std::string getName();
		void setName(std::string name);
		bool isVisible();
		void setVisible(bool visible);
		unsigned int getLayer();
		void setLayer(unsigned int layer);
		//For Component Transform
		// Position 
		;
		std::vector<float> getPosition();
		void setPosition(std::array<float,2> Pos);
		void addPositionOffset(std::array<float, 2> Offset);
		void addPositionXOffset(float x);
		void addPositionYOffset(float y);
		// Rotation
		float getAngle();
		void setAngle(float Angle);
		void addAngleOffset(float Angle);
		// Scale
		std::vector<float> getScale();
		void setScale(std::array<float, 2> Scale);
		void setScaleX(float x);
		void setScaleY(float y);
		void addScaleOffset(std::array<float, 2> Offset);
		void addScaleXOffset(float x);
		void addScaleYOffset(float y);
		// For Component Texture
		std::string getTexturePath();
		void loadTextureFromPath(std::string path);
		void setTextureRect(std::array<float, 4> rect);
		std::vector<float> getTextureRect();
		void setSize(std::array<float,2> size);
		std::vector<float> getSize();
		float getTextureWidth();
		float getTextureHeight();
		std::vector<float> getTextureSize();
		// ColorMod
		std::vector<float> getColorMod();
		void setColorMod(std::array<float, 4> color);
		// Scale Mode
		 SDL_ScaleMode getScaleMod();
		 void setScaleMod(SDL_ScaleMode scaleMode);
		 // Blend Mode
		 TextureComponent::BlendMode getBlendMode();
		 void setBlendMode(TextureComponent::BlendMode blendMode);
		 // Flip Mode
		 TextureComponent::FlipMode getFlipMode();
		 void setFlipMode(TextureComponent::FlipMode flipMode);
		 // Shape
		 TextureComponent::Shape getShape();
		 // For Component RigidBody
		 // Velocity
		 std::vector<float> getVelocity();
		 void setVelocity(std::array<float, 2> velocity);
		 void addVelocityOffset(std::array<float, 2> offset);
		 void addVelocityXOffset(float x);
		 void addVelocityYOffset(float y);
		 float getAngularVelocity();
		 void setAngularVelocity(float velocity);
		 void addAngularVelocityOffset(float offset);
		 // Gravity Scale
		 float getGravityScale();
		 void setGravityScale(float scale);
		 // RigidBody Props
		 // Friction
		 float getFriction();
		 void setFriction(float friction);
		 // Restitution
		 float getRestitution();
		 void setRestitution(float restitution);
		 // Density
		 float getDensity();
		 void setDensity(float density);
		 // Sensor
		 bool isSensor();
		 void setSensor(bool sensor);
		 // Restitution Threshold
		 float getRestitutionThreshold();
		 void setRestitutionThreshold(float threshold);
		 // Fixed Rotation
		 bool getFixedRotation();
		 void setFixedRotation(bool fixedRotation);
		 // Force
		 void applyForce(std::array<float, 2> force);
		 void applyForceToCenter(std::array<float, 2> force);
		 // Torque
		 void applyTorque(float torque);
		 // Impulse
		 void applyImpulse(std::array<float, 2> impulse);
		 void applyImpulseToCenter(std::array<float, 2> impulse);
		 void applyAngularImpulse(float impulse);
		 
	private:
		std::weak_ptr<GObject> _Object;
		std::weak_ptr<TextureComponent> _TextureComponent;
		std::weak_ptr<RigidBodyComponent> _RigidBodyComponent;
		std::weak_ptr<TransformComponent> _TransformComponent;
	};
}
