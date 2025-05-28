#include "../framework/object/object.hpp"
#include "../framework/level/level.hpp"
#include "../framework/world/world.hpp"
#include "../framework/component/component.hpp"
#include "../framework/component/transform/transform_component.hpp"
#include "../framework/component/rigidbody/rigidbody_component.hpp"
#include "../../resources/asset_manager/asset.hpp"
#include "object_wrapper.hpp"


namespace GearX {
	template<typename T>
	static std::shared_ptr<T> getComponent(std::shared_ptr<GObject>& object) {
		auto& com = object->getComponentByTypeName(rttr::type::get<T>().get_name());
		if (com) {
			auto& component = std::dynamic_pointer_cast<T>(com);
			return component;
		}
		else {
			return nullptr;
		}
	}
	template<size_t N>
	std::array<float, N> cvtArray(const sol::table& t) {
		std::array<float, N> arr;
		for (size_t i = 0; i < N; ++i) {
			arr[i] = t[i + 1].get<float>(); // Lua索引从1开始
		}
		return arr;
	}
	// 构造函数
	GObjectWrapper::GObjectWrapper(std::shared_ptr<GObject>& object) :
		_Object(object) {
		if (object) {
			_TransformComponent = getComponent<TransformComponent>(object);
			_TextureComponent = getComponent<TextureComponent>(object);
			_RigidBodyComponent = getComponent<RigidBodyComponent>(object);
		}
	}

	bool GObjectWrapper::isValid() const
	{
		return !_Object.expired();
	}

	bool GObjectWrapper::isValidTexture() const
	{
		return !_TextureComponent.expired();
	}

	bool GObjectWrapper::isValidRigidBody() const
	{
		return !_RigidBodyComponent.expired();
	}

	bool GObjectWrapper::isValidTransform() const
	{
		return !_TransformComponent.expired();
	}

	GObjectID GObjectWrapper::getID() {
		if (isValid()) {
			return _Object.lock()->getID();
		}
		else {
			return 0;
		}
	}

	std::string GObjectWrapper::getName(){
		if (isValid()) {
			return _Object.lock()->getName();
		}
		else {
			return std::string();
		}
	}

	void GObjectWrapper::setName(std::string name){
		if (isValid()) {
			_Object.lock()->setName(name);
		}
	}
	//保留接口
	bool GObjectWrapper::isVisible(){
		return true;
	}

	void GObjectWrapper::setVisible(bool visible){
	}

	unsigned int GObjectWrapper::getLayer(){
		if (isValid()) {
			auto& level = _Object.lock()->getLevel();
			Uint16 layer = 0;
			level->getLayerFromID(_Object.lock()->getID(),layer);
			return layer;
		}
		else {
			return 0;
		}
	}

	void GObjectWrapper::setLayer(unsigned int layer){
		if (isValid()) {
			auto& level = _Object.lock()->getLevel();
			level->setObjectToLayer(_Object.lock()->getID(), layer);
		}
	}

	/* ​** ​* ​** ​* ​** ​** Transform 组件 ​** ​* ​** ​* ​** ​** */
	// 位置
	std::vector<float> GObjectWrapper::getPosition() {
		if (isValidTexture() && isValidTransform()) {
			return { _TransformComponent.lock()->getPosition()[0] + _TextureComponent.lock()->getDstRect()[2] / 2.0f,
				_TransformComponent.lock()->getPosition()[1] + _TextureComponent.lock()->getDstRect()[3] / 2.0f };
		}
		else if (isValidTransform()) {
			return { _TransformComponent.lock()->getPosition()[0],
			_TransformComponent.lock()->getPosition()[1] };
		}
		else
			return { 0.0f, 0.0f };
	}
	void GObjectWrapper::setPosition(std::array<float, 2> Pos) {
		if (isValidTexture() && isValidTransform()) {
			_TransformComponent.lock()->setPosition({ Pos[0] - _TextureComponent.lock()->getDstRect()[2] / 2.0f,
				Pos[1] - _TextureComponent.lock()->getDstRect()[3] / 2.0f });
		}
		else if (isValidTransform()) {
			_TransformComponent.lock()->setPosition(Pos);
		}
	}
	void GObjectWrapper::addPositionOffset(std::array<float, 2> Offset) {
		if (isValidTransform()) {
			auto transform = _TransformComponent.lock();
			auto currentPos = transform->getPosition();
			transform->setPosition({ currentPos[0] + Offset[0], currentPos[1] + Offset[1] });
		}
	}
	void GObjectWrapper::addPositionXOffset(float x) {
		addPositionOffset({ x, 0.0f });
	}
	void GObjectWrapper::addPositionYOffset(float y) {
		addPositionOffset({ 0.0f, y });
	}

	// 旋转
	float GObjectWrapper::getAngle() {
		if (isValidTransform()) {
			return _TransformComponent.lock()->getRotation();
		}
		else
			return 0.0f;
	}
	void GObjectWrapper::setAngle(float Angle) {
		if (isValidTransform()) {
			_TransformComponent.lock()->setRotation(Angle);
		}
	}
	void GObjectWrapper::addAngleOffset(float Angle) {
		if (isValidTransform()) {
			auto transform = _TransformComponent.lock();
			auto currentAngle = transform->getRotation();
			transform->setRotation(currentAngle + Angle);
		}
	}

	// 缩放
	std::vector<float> GObjectWrapper::getScale() {
		if (isValidTransform()) {
			return { _TransformComponent.lock()->getScale()[0],
			_TransformComponent.lock()->getScale()[1] };
		}
		else
			return { 0.0f, 0.0f };
	}
	void GObjectWrapper::setScale(std::array<float, 2> Scale) {
		if (isValidTransform()) {
			_TransformComponent.lock()->setScale(Scale);
		}
	}
	void GObjectWrapper::setScaleX(float x) {
		if (isValidTransform()) {
			auto transform = _TransformComponent.lock();
			auto currentScale = transform->getScale();
			transform->setScale({ x, currentScale[1] });
		}
	}
	void GObjectWrapper::setScaleY(float y) {
		if (isValidTransform()) {
			auto transform = _TransformComponent.lock();
			auto currentScale = transform->getScale();
			transform->setScale({ currentScale[0], y });
		}
	}
	void GObjectWrapper::addScaleOffset(std::array<float, 2> Offset) {
		if (isValidTransform()) {
			auto transform = _TransformComponent.lock();
			auto currentScale = transform->getScale();
			transform->setScale({ currentScale[0] + Offset[0], currentScale[1] + Offset[1] });
		}
	}
	void GObjectWrapper::addScaleXOffset(float x) {
		if (isValidTransform()) {
			auto transform = _TransformComponent.lock();
			auto currentScale = transform->getScale();
			transform->setScale({ currentScale[0] + x, currentScale[1] });
		}
	}
	void GObjectWrapper::addScaleYOffset(float y) {
		if (isValidTransform()) {
			auto transform = _TransformComponent.lock();
			auto currentScale = transform->getScale();
			transform->setScale({ currentScale[0], currentScale[1] + y });
		}
	}

	/* *​ * ​ * *​ * ​ * *​ * *Texture 组件 ​ * *​ * ​ * *​ * ​ * *​ * */
	std::string GObjectWrapper::getTexturePath() {
		if (isValidTexture()) {
			return _TextureComponent.lock()->getTextureAssetUrl();
		}
		else
			return std::string();
	}
	void GObjectWrapper::loadTextureFromPath(std::string path) {
		if (isValidTexture()) {
			_TextureComponent.lock()->setTextureAssetUrl(path);
		}
	}
	void GObjectWrapper::setTextureRect(std::array<float, 4> rect) {
		if (isValidTexture()) {
			_TextureComponent.lock()->setSrcRect(rect);
		}
	}
	std::vector<float> GObjectWrapper::getTextureRect() {
		if (isValidTexture()) {
			std::vector<float> vec;
			for (auto& i : _TextureComponent.lock()->getSrcRect()) {
				vec.push_back(i);
			}
			return vec;
		}
		else
			return { 0.0f, 0.0f, 0.0f, 0.0f };
	}
	float GObjectWrapper::getTextureWidth() {
		if (isValidTexture()) {
			return _TextureComponent.lock()->getDstRect()[2];
		}
		else
			return 0.0f;
	}
	float GObjectWrapper::getTextureHeight() {
		if (isValidTexture()) {
			return _TextureComponent.lock()->getDstRect()[3];
		}
		else
			return 0.0f;
	}
	std::vector<float> GObjectWrapper::getTextureSize() {
		if (isValidTexture()) {
			return { _TextureComponent.lock()->getTextureSize()[0]
				, _TextureComponent.lock()->getTextureSize()[1] };
		}
		else
			return { 0.0f, 0.0f };
	}

	// 颜色修改
	std::vector<float> GObjectWrapper::getColorMod() {
		if (isValidTexture()) {
			auto& color = _TextureComponent.lock()->getColorMod().color;
			return { color[0],color[1],color[2],color[3] };
		}
		else
			return { 0.0f, 0.0f, 0.0f, 0.0f };
	}
	void GObjectWrapper::setColorMod(std::array<float, 4> color) {
		if (isValidTexture()) {
			ColorType colorType;
			colorType.color = color;
			_TextureComponent.lock()->setColorMod(colorType);
		}
	}

	// 缩放模式
	SDL_ScaleMode GObjectWrapper::getScaleMod() {
		if (isValidTexture()) {
			return _TextureComponent.lock()->getScaleMode();
		}
		else
			return SDL_ScaleMode::SDL_SCALEMODE_INVALID;
	}
	void GObjectWrapper::setScaleMod(SDL_ScaleMode scaleMode) {
		if (isValidTexture()) {
			_TextureComponent.lock()->setScaleMode(scaleMode);
		}
	}

	// 混合模式
	TextureComponent::BlendMode GObjectWrapper::getBlendMode() {
		if (isValidTexture()) {
			return _TextureComponent.lock()->getBlendMode();
		}
		else
			return TextureComponent::BlendMode::None;
	}
	void GObjectWrapper::setBlendMode(TextureComponent::BlendMode blendMode) {
		if (isValidTexture()) {
			_TextureComponent.lock()->setBlendMode(blendMode);
		}
	}

	// 翻转模式
	TextureComponent::FlipMode GObjectWrapper::getFlipMode() {
		if (isValidTexture()) {
			return _TextureComponent.lock()->getFlipMode();
		}
		else
			return TextureComponent::FlipMode::None;
	}
	void GObjectWrapper::setFlipMode(TextureComponent::FlipMode flipMode) {
		if (isValidTexture()) {
			_TextureComponent.lock()->setFlipMode(flipMode);
		}
	}

	// 形状
	TextureComponent::Shape GObjectWrapper::getShape() {
		if (isValidTexture()) {
			return _TextureComponent.lock()->getShape();
		}
		else
			return TextureComponent::Shape::Rect;
	}

	/* ​** ​* ​** ​* ​** ​** RigidBody 组件 ​** ​* ​** ​* ​** ​**/
	// 速度
	std::vector<float> GObjectWrapper::getVelocity() {
		if (isValidRigidBody()) {
			return { _RigidBodyComponent.lock()->getLinearVelocity()[0],
				_RigidBodyComponent.lock()->getLinearVelocity()[1] };
		}
		else
			return { 0.0f, 0.0f };
	}
	void GObjectWrapper::setVelocity(std::array<float, 2> velocity) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->setLinearVelocity(velocity);
		}
	}
	void GObjectWrapper::addVelocityOffset(std::array<float, 2> offset) {
		if (isValidRigidBody()) {
			auto rigidBody = _RigidBodyComponent.lock();
			auto currentVelocity = rigidBody->getLinearVelocity();
			rigidBody->setLinearVelocity({ currentVelocity[0] + offset[0], currentVelocity[1] + offset[1] });
		}
	}
	void GObjectWrapper::addVelocityXOffset(float x) {
		if (isValidRigidBody()) {
			auto rigidBody = _RigidBodyComponent.lock();
			auto currentVelocity = rigidBody->getLinearVelocity();
			rigidBody->setLinearVelocity({ currentVelocity[0] + x, currentVelocity[1] });
		}
	}
	void GObjectWrapper::addVelocityYOffset(float y) {
		if (isValidRigidBody()) {
			auto rigidBody = _RigidBodyComponent.lock();
			auto currentVelocity = rigidBody->getLinearVelocity();
			rigidBody->setLinearVelocity({ currentVelocity[0], currentVelocity[1] + y });
		}
	}

	// 角速度
	float GObjectWrapper::getAngularVelocity() {
		if (isValidRigidBody()) {
			return _RigidBodyComponent.lock()->getAngularVelocity();
		}
		else
			return 0.0f;
	}
	void GObjectWrapper::setAngularVelocity(float velocity) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->setAngularVelocity(velocity);
		}
	}
	void GObjectWrapper::addAngularVelocityOffset(float offset) {
		if (isValidRigidBody()) {
			auto rigidBody = _RigidBodyComponent.lock();
			auto currentVelocity = rigidBody->getAngularVelocity();
			rigidBody->setAngularVelocity(currentVelocity + offset);
		}
	}

	// 重力缩放
	float GObjectWrapper::getGravityScale() {
		if (isValidRigidBody()) {
			return _RigidBodyComponent.lock()->getGravityScale();
		}
		else
			return 0.0f;
	}
	void GObjectWrapper::setGravityScale(float scale) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->setGravityScale(scale);
		}
	}

	// 刚体属性
	float GObjectWrapper::getFriction() {
		if (isValidRigidBody()) {
			return _RigidBodyComponent.lock()->getFriction();
		}
		else
			return 0.0f;
	}
	void GObjectWrapper::setFriction(float friction) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->setFriction(friction);
		}
	}
	float GObjectWrapper::getRestitution() {
		if (isValidRigidBody()) {
			return _RigidBodyComponent.lock()->getRestitution();
		}
		else
			return 0.0f;
	}
	void GObjectWrapper::setRestitution(float restitution) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->setRestitution(restitution);
		}
	}
	float GObjectWrapper::getDensity() {
		if (isValidRigidBody()) {
			return _RigidBodyComponent.lock()->getDensity();
		}
		else
			return 0.0f;
	}
	void GObjectWrapper::setDensity(float density) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->setDensity(density);
		}
	}

	// 传感器
	bool GObjectWrapper::isSensor() {
		if (isValidRigidBody()) {
			return _RigidBodyComponent.lock()->isSensor();
		}
		else
			return false;
	}
	void GObjectWrapper::setSensor(bool sensor) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->setSensor(sensor);
		}
	}

	// 恢复阈值
	float GObjectWrapper::getRestitutionThreshold() {
		if (isValidRigidBody()) {
			return _RigidBodyComponent.lock()->getRestitutionThreshold();
		}
		else
			return 0.0f;
	}
	void GObjectWrapper::setRestitutionThreshold(float threshold) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->setRestitutionThreshold(threshold);
		}
	}

	// 固定旋转
	bool GObjectWrapper::getFixedRotation() {
		if (isValidRigidBody()) {
			return _RigidBodyComponent.lock()->getFixedRotation();
		}
		else
			return false;
	}
	void GObjectWrapper::setFixedRotation(bool fixedRotation) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->setFixedRotation(fixedRotation);
		}
	}
	// 力和冲量
	void GObjectWrapper::applyForce(std::array<float, 2> force) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->applyForce(force);
		}
	}
	void GObjectWrapper::applyForceToCenter(std::array<float, 2> force) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->applyForceToCenter(force);
		}
	}
	void GObjectWrapper::applyTorque(float torque) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->applyTorque(torque);
		}
	}
	void GObjectWrapper::applyImpulse(std::array<float, 2> impulse) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->applyImpulse(impulse);
		}
	}
	void GObjectWrapper::applyImpulseToCenter(std::array<float, 2> impulse) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->applyImpulseToCenter(impulse);
		}
	}
	void GObjectWrapper::applyAngularImpulse(float impulse) {
		if (isValidRigidBody()) {
			_RigidBodyComponent.lock()->applyAngularImpulse(impulse);
		}
	}
	// Lua 注册函数	
	// Lua 注册函数	
	void GObjectWrapper::RegisterEnum(sol::state& lua) {
		// 注册枚举类型
		lua.new_enum("ScaleMode",
			"Invalid", SDL_ScaleMode::SDL_SCALEMODE_INVALID,
			"Linear", SDL_ScaleMode::SDL_SCALEMODE_LINEAR,
			"Nearest", SDL_ScaleMode::SDL_SCALEMODE_NEAREST
		);

		lua.new_enum("BlendMode",
			"None", TextureComponent::BlendMode::None,
			"Add", TextureComponent::BlendMode::Add,
			"AddPremultiplied", TextureComponent::BlendMode::AddPremultiplied,
			"Blend", TextureComponent::BlendMode::Blend,
			"BlendPremultiplied", TextureComponent::BlendMode::BlendPremultiplied,
			"Mod", TextureComponent::BlendMode::Mod
		);
		lua.new_enum("FlipMode",
			"None", TextureComponent::FlipMode::None,
			"Horizontal", TextureComponent::FlipMode::Horizontal,
			"Vertical", TextureComponent::FlipMode::Vertical,
			"VerticalAndHorizontal", TextureComponent::FlipMode::VerticalAndHorizontal
		);
		lua.new_enum("Shape",
			"Rect", TextureComponent::Shape::Rect,
			"Circle", TextureComponent::Shape::Circle
		);
	}
	sol::table& GObjectWrapper::getTable(sol::state& state) {
		auto& table = state.create_table();
		auto& transform_table = state.create_table();
		auto& texture_table = state.create_table();
		auto& rigidbody_table = state.create_table();
		// 对象ID
		table["getID"] = [this]() {return this->getID(); };
		table["getName"] =[this]() {return this->getName(); };
		table["setName"] = [this](const std::string& name) { this->setName(name); };
		table["getLayer"] = [this]() { return this->getLayer(); };
		table["setLayer"] = [this](Uint16 layer) { this->setLayer(layer); };
		table["getVisible"] = [this]() { return this->isVisible(); };
		table["setVisible"] = [this](bool) {this->setVisible(true); };
		// 注册基础状态检查函数
		table["isValid"] = [this]() { return this->isValid(); };
		table["isValidTexture"] = [this]() { return this->isValidTexture(); };
		table["isValidRigidBody"] = [this]() { return this->isValidRigidBody(); };
		table["isValidTransform"] = [this]() { return this->isValidTransform(); };

		/* ​**​*​**​*​**​*​**​*​**​* Transform 组件 ​**​*​**​*​**​*​**​*​**​* */
		// 位置相关函数
		transform_table["getPosition"] = sol::as_table([this]() { return this->getPosition(); });
		transform_table["setPosition"] = [this](const sol::table& pos) {
			this->setPosition(cvtArray<2>(pos));
			};
		transform_table["addPositionOffset"] = [this](const sol::table& offset) {
			this->addPositionOffset(cvtArray<2>(offset));
			};
		transform_table["addPositionXOffset"] = [this](float x) { this->addPositionXOffset(x); };
		transform_table["addPositionYOffset"] = [this](float y) { this->addPositionYOffset(y); };

		// 旋转相关函数
		transform_table["getAngle"] = [this]() { return this->getAngle(); };
		transform_table["setAngle"] = [this](float angle) { this->setAngle(angle); };
		transform_table["addAngleOffset"] = [this](float offset) { this->addAngleOffset(offset); };

		// 缩放相关函数
		transform_table["getScale"] = sol::as_table([this]() { return this->getScale(); });
		transform_table["setScale"] = [this](const sol::table& scale) {
			this->setScale(cvtArray<2>(scale));
			};
		transform_table["setScaleX"] = [this](float x) { this->setScaleX(x); };
		transform_table["setScaleY"] = [this](float y) { this->setScaleY(y); };
		transform_table["addScaleOffset"] = [this](const sol::table& offset) {
			this->addScaleOffset(cvtArray<2>(offset));
			};
		transform_table["addScaleXOffset"] = [this](float x) { this->addScaleXOffset(x); };
		transform_table["addScaleYOffset"] = [this](float y) { this->addScaleYOffset(y); };

		/* ​**​*​**​*​**​*​**​*​**​* Texture 组件 ​**​*​**​*​**​*​**​*​**​* */
		// 纹理路径与尺寸
		texture_table["getTexturePath"] = [this]() { return this->getTexturePath(); };
		texture_table["loadTextureFromPath"] = [this](const std::string& path) { return this->loadTextureFromPath(path); };
		texture_table["getTextureRect"] = sol::as_table([this]() { return this->getTextureRect(); });
		texture_table["setTextureRect"] = [this](const sol::table& rect) {
			this->setTextureRect(cvtArray<4>(rect));
			};
		texture_table["getTextureWidth"] = [this]() { return this->getTextureWidth(); };
		texture_table["getTextureHeight"] = [this]() { return this->getTextureHeight(); };
		texture_table["getTextureSize"] = sol::as_table([this]() { return this->getTextureSize(); });

		// 颜色与模式控制
		texture_table["getColorMod"] = sol::as_table([this]() { return this->getColorMod(); });
		texture_table["setColorMod"] = [this](const sol::table& color) {
			this->setColorMod(cvtArray<4>(color));
			};
		texture_table["getScaleMod"] = [this]() { return this->getScaleMod(); };
		texture_table["setScaleMod"] = [this](SDL_ScaleMode scale) { this->setScaleMod(scale); };
		texture_table["getBlendMode"] = [this]() { return this->getBlendMode(); };
		texture_table["setBlendMode"] = [this](TextureComponent::BlendMode mode) { this->setBlendMode(mode); };
		texture_table["getFlipMode"] = [this]() { return this->getFlipMode(); };
		texture_table["setFlipMode"] = [this](TextureComponent::FlipMode mode) { this->setFlipMode(mode); };
		texture_table["getShape"] = [this]() { return this->getShape(); };

		/* ​**​*​**​*​**​*​**​*​**​* RigidBody 组件 ​**​*​**​*​**​*​**​*​**​* */
		// 速度与角速度
		rigidbody_table["getVelocity"] = sol::as_table([this]() { return this->getVelocity(); });
		rigidbody_table["setVelocity"] = [this](const sol::table& vel) {
			this->setVelocity(cvtArray<2>(vel));
			};
		rigidbody_table["addVelocityOffset"] = [this](const sol::table& offset) {
			this->addVelocityOffset(cvtArray<2>(offset));
			};
		rigidbody_table["addVelocityXOffset"] = [this](float x) { this->addVelocityXOffset(x); };
		rigidbody_table["addVelocityYOffset"] = [this](float y) { this->addVelocityYOffset(y); };
		rigidbody_table["getAngularVelocity"] = [this]() { return this->getAngularVelocity(); };
		rigidbody_table["setAngularVelocity"] = [this](float vel) { this->setAngularVelocity(vel); };
		rigidbody_table["addAngularVelocityOffset"] = [this](float offset) { this->addAngularVelocityOffset(offset); };

		// 物理属性
		rigidbody_table["getGravityScale"] = [this]() { return this->getGravityScale(); };
		rigidbody_table["setGravityScale"] = [this](float scale) { this->setGravityScale(scale); };
		rigidbody_table["getFriction"] = [this]() { return this->getFriction(); };
		rigidbody_table["setFriction"] = [this](float friction) { this->setFriction(friction); };
		rigidbody_table["getRestitution"] = [this]() { return this->getRestitution(); };
		rigidbody_table["setRestitution"] = [this](float restitution) { this->setRestitution(restitution); };
		rigidbody_table["getDensity"] = [this]() { return this->getDensity(); };
		rigidbody_table["setDensity"] = [this](float density) { this->setDensity(density); };
		rigidbody_table["isSensor"] = [this]() { return this->isSensor(); };
		rigidbody_table["setSensor"] = [this](bool sensor) { this->setSensor(sensor); };
		rigidbody_table["getRestitutionThreshold"] = [this]() { return this->getRestitutionThreshold(); };
		rigidbody_table["setRestitutionThreshold"] = [this](float threshold) { this->setRestitutionThreshold(threshold); };
		rigidbody_table["getFixedRotation"] = [this]() { return this->getFixedRotation(); };
		rigidbody_table["setFixedRotation"] = [this](bool fixed) { this->setFixedRotation(fixed); };

		// 力与冲量
		rigidbody_table["applyForce"] = [this](const sol::table& force) {
			this->applyForce(cvtArray<2>(force));
			};
		rigidbody_table["applyForceToCenter"] = [this](const sol::table& force) {
			this->applyForceToCenter(cvtArray<2>(force));
			};
		rigidbody_table["applyTorque"] = [this](float torque) { this->applyTorque(torque); };
		rigidbody_table["applyImpulse"] = [this](const sol::table& impulse) {
			this->applyImpulse(cvtArray<2>(impulse));
			};
		rigidbody_table["applyImpulseToCenter"] = [this](const sol::table& impulse) {
			this->applyImpulseToCenter(cvtArray<2>(impulse));
			};
		rigidbody_table["applyAngularImpulse"] = [this](float impulse) { this->applyAngularImpulse(impulse); };
		table["Transform"] = transform_table;
		table["Texture"] = texture_table;
		table["RigidBody"] = rigidbody_table;
		TableOfObjects.push_back(table);
		return TableOfObjects.back();
	}
	sol::table& GObjectWrapper::getEmptyTable(sol::state& state) {
		static auto& table = state.create_table();
		// 对象ID
		static bool isInit = false;
		if (!isInit) {
			auto& transform_table = state.create_table();
			auto& texture_table = state.create_table();
			auto& rigidbody_table = state.create_table();
			// 对象ID
			table["getID"] = []() { return 0; };
			table["getName"] = []() { return ""; };
			table["setName"] = [](const std::string&) {};
			table["getLayer"] = []() { return 0; };
			table["setLayer"] = [](int) {};
			table["getVisible"] = []() { return false; };
			table["setVisible"] = [](bool) {};
			// 注册基础状态检查函数
			table["isValid"] = []() { return false; };
			table["isValidTexture"] = []() { return false; };
			table["isValidRigidBody"] = []() { return false; };
			table["isValidTransform"] = []() { return false; };

			/* Transform 组件 */
			// 位置相关函数
			transform_table["getPosition"] = sol::as_table([]() { return std::array<float, 2>{0, 0}; });
			transform_table["setPosition"] = [](const sol::table&) {};
			transform_table["addPositionOffset"] = [](const sol::table&) {};
			transform_table["addPositionXOffset"] = [](float) {};
			transform_table["addPositionYOffset"] = [](float) {};

			// 旋转相关函数
			transform_table["getAngle"] = []() { return 0.0f; };
			transform_table["setAngle"] = [](float) {};
			transform_table["addAngleOffset"] = [](float) {};

			// 缩放相关函数
			transform_table["getScale"] = sol::as_table([]() { return std::array<float, 2>{0, 0}; });
			transform_table["setScale"] = [](const sol::table&) {};
			transform_table["setScaleX"] = [](float) {};
			transform_table["setScaleY"] = [](float) {};
			transform_table["addScaleOffset"] = [](const sol::table&) {};
			transform_table["addScaleXOffset"] = [](float) {};
			transform_table["addScaleYOffset"] = [](float) {};

			/* Texture 组件 */
			// 纹理路径与尺寸
			texture_table["getTexturePath"] = []() { return ""; };
			texture_table["loadTextureFromPath"] = [](const std::string&) { return false; };
			texture_table["getTextureRect"] = sol::as_table([]() { return std::array<float, 4>{0, 0, 0, 0}; });
			texture_table["setTextureRect"] = [](const sol::table&) {};
			texture_table["getTextureWidth"] = []() { return 0; };
			texture_table["getTextureHeight"] = []() { return 0; };
			texture_table["getTextureSize"] = sol::as_table([]() { return std::array<float, 2>{0, 0}; });

			// 颜色与模式控制
			texture_table["getColorMod"] = sol::as_table([]() { return std::array<float, 4>{0, 0, 0, 0}; });
			texture_table["setColorMod"] = [](const sol::table&) {};
			texture_table["getScaleMod"] = []() { return SDL_ScaleMode::SDL_SCALEMODE_INVALID; };
			texture_table["setScaleMod"] = [](SDL_ScaleMode) {};
			texture_table["getBlendMode"] = []() { return TextureComponent::BlendMode::None; };
			texture_table["setBlendMode"] = [](TextureComponent::BlendMode) {};
			texture_table["getFlipMode"] = []() { return TextureComponent::FlipMode::None; };
			texture_table["setFlipMode"] = [](TextureComponent::FlipMode) {};
			texture_table["getShape"] = []() { return TextureComponent::Shape::Rect; };

			/* RigidBody 组件 */
			// 速度与角速度
			rigidbody_table["getVelocity"] = sol::as_table([]() { return std::array<float, 2>{0, 0}; });
			rigidbody_table["setVelocity"] = [](const sol::table&) {};
			rigidbody_table["addVelocityOffset"] = [](const sol::table&) {};
			rigidbody_table["addVelocityXOffset"] = [](float) {};
			rigidbody_table["addVelocityYOffset"] = [](float) {};
			rigidbody_table["getAngularVelocity"] = []() { return 0.0f; };
			rigidbody_table["setAngularVelocity"] = [](float) {};
			rigidbody_table["addAngularVelocityOffset"] = [](float) {};

			// 物理属性
			rigidbody_table["getGravityScale"] = []() { return 0.0f; };
			rigidbody_table["setGravityScale"] = [](float) {};
			rigidbody_table["getFriction"] = []() { return 0.0f; };
			rigidbody_table["setFriction"] = [](float) {};
			rigidbody_table["getRestitution"] = []() { return 0.0f; };
			rigidbody_table["setRestitution"] = [](float) {};
			rigidbody_table["getDensity"] = []() { return 0.0f; };
			rigidbody_table["setDensity"] = [](float) {};
			rigidbody_table["isSensor"] = []() { return false; };
			rigidbody_table["setSensor"] = [](bool) {};
			rigidbody_table["getRestitutionThreshold"] = []() { return 0.0f; };
			rigidbody_table["setRestitutionThreshold"] = [](float) {};
			rigidbody_table["getFixedRotation"] = []() { return false; };
			rigidbody_table["setFixedRotation"] = [](bool) {};

			// 力与冲量
			rigidbody_table["applyForce"] = [](const sol::table&) {};
			rigidbody_table["applyForceToCenter"] = [](const sol::table&) {};
			rigidbody_table["applyTorque"] = [](float) {};
			rigidbody_table["applyImpulse"] = [](const sol::table&) {};
			rigidbody_table["applyImpulseToCenter"] = [](const sol::table&) {};
			rigidbody_table["applyAngularImpulse"] = [](float) {};
			table["Transform"] = transform_table;
			table["Texture"] = texture_table;
			table["RigidBody"] = rigidbody_table;
			TableOfObjects.push_back(table);
			isInit = true;
		}
		return table;
	}

}