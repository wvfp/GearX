#include "texture_component.hpp"
#include "../transform/transform_component.hpp"

namespace GearX {
	// 注册枚举类型（需在类注册前完成）
	RTTR_REGISTRATION{
rttr::registration::enumeration<TextureComponent::BlendMode>(u8"BlendMode")
		(
		rttr::value(u8"无", TextureComponent::BlendMode::None),
		rttr::value(u8"混合", TextureComponent::BlendMode::Blend),
		rttr::value(u8"相加", TextureComponent::BlendMode::Add),
		rttr::value(u8"调制", TextureComponent::BlendMode::Mod),
		rttr::value(u8"预乘相加",  TextureComponent::BlendMode::AddPremultiplied),
		rttr::value(u8"预乘混合", TextureComponent::BlendMode::BlendPremultiplied)
		);
	// 注册 SDL_FlipMode 枚举
	rttr::registration::enumeration<TextureComponent::FlipMode>("FlipMode")
		(
			rttr::value(u8"无", TextureComponent::FlipMode::None),
			rttr::value(u8"水平翻转", TextureComponent::FlipMode::Horizontal),
			rttr::value(u8"垂直翻转", TextureComponent::FlipMode::Vertical),
			rttr::value(u8"水平垂直翻转", TextureComponent::FlipMode::VerticalAndHorizontal)
		);
	// 注册 SDL_ScaleMode 枚举
	rttr::registration::enumeration<SDL_ScaleMode>("ScaleMode")
		(
			rttr::value(u8"无效",SDL_ScaleMode::SDL_SCALEMODE_INVALID),
			rttr::value(u8"线性",SDL_ScaleMode::SDL_SCALEMODE_LINEAR),
			rttr::value(u8"邻近", SDL_ScaleMode::SDL_SCALEMODE_NEAREST)
		);
	rttr::registration::enumeration<TextureComponent::Shape>("Shape")
		(
			rttr::value(u8"矩形", TextureComponent::Shape::Rect),
			rttr::value(u8"圆形", TextureComponent::Shape::Circle)
			//rttr::value(u8"多边形",TextureComponent::Shape::Polygon)
		);
	rttr::registration::class_<ColorType>("ColorType")
		.property(u8"颜色", &GearX::ColorType::color);
	// 注册 TextureComponent 类
	rttr::registration::class_<GearX::TextureComponent>("TextureComponent")
		.constructor<>() // 注册默认构造函数
		// 注册属性（包含私有成员）
		.property(u8"源矩形", &GearX::TextureComponent::getSrcRect, &GearX::TextureComponent::setSrcRect)
		.property(u8"目标矩形", &GearX::TextureComponent::getDstRect, &GearX::TextureComponent::setDstRect)
		.property(u8"纹理路径", &GearX::TextureComponent::getTextureAssetUrl, &GearX::TextureComponent::setTextureAssetUrl)
		.property(u8"尺寸", &GearX::TextureComponent::texture_size)
		.property(u8"颜色调制", &GearX::TextureComponent::getColorMod, &GearX::TextureComponent::setColorMod)
		.property(u8"混合模式", &GearX::TextureComponent::getBlendMode, &GearX::TextureComponent::setBlendMode)
		.property(u8"翻转模式", &GearX::TextureComponent::getFlipMode, &GearX::TextureComponent::setFlipMode)
		.property(u8"缩放模式", &GearX::TextureComponent::getScaleMode, &GearX::TextureComponent::setScaleMode)
		.property(u8"形状",&GearX::TextureComponent::getShape, &GearX::TextureComponent::setShape)
		// 注册方法
		.method("addComponentTo", &GearX::TextureComponent::addComponentTo);
	};
	void TextureComponent::addComponentTo(std::shared_ptr<GObject> obj) {
		if (obj) {
			auto transform_com = obj->getComponentByTypeName(rttr::type::get<TransformComponent>().get_name());
			if (transform_com == nullptr) {
				TransformComponent::addComponentTo(obj);
			}
			auto com1 = obj->getComponentByTypeName(rttr::type::get<TransformComponent>().get_name());
			auto transform = std::dynamic_pointer_cast<TransformComponent>(com1);
			auto com = std::make_shared<TextureComponent>();
			com->setDstRectX(transform->getPositionX());
			com->setDstRectY(transform->getPositionY());
			obj->addComponent(com);
		}
	}
	std::shared_ptr<Component> GearX::TextureComponent::copy() {
		auto com = std::make_shared<TextureComponent>();
		*com = *this;
		com->m_parentObject.reset();
		return com;
	}
	//---------- 序列化实现 ----------
	template<class Archive>
	void TextureComponent::serialize(Archive& archive) {
		uint8_t scale_mode = static_cast<uint8_t>(scaleMode);
		uint8_t shape_ = static_cast<uint8_t>(shape);
		uint8_t blend_mode = static_cast<uint8_t>(blendMode);
		uint8_t flip_mode = static_cast<uint8_t>(flipMode);
		Component::serialize(archive); // 必须调用基类
		archive(
			cereal::make_nvp("texture_asset", m_texture_asset),
			cereal::make_nvp("alpha_mod", alphaMod),
			cereal::make_nvp("red_mod", redMod),
			cereal::make_nvp("green_mod", greenMod),
			cereal::make_nvp("blue_mod", blueMod),
			cereal::make_nvp("scale_mod", scale_mode),
			cereal::make_nvp("blend_mode", blend_mode),
			cereal::make_nvp("flip_mode", flip_mode),
			cereal::make_nvp("shape", shape_),
			cereal::make_nvp("vertices", vertices),
			cereal::make_nvp("src_rect.x", srcRect.x),
			cereal::make_nvp("src_rect.y", srcRect.y),
			cereal::make_nvp("src_rect.w", srcRect.w),
			cereal::make_nvp("src_rect.h", srcRect.h),
			cereal::make_nvp("dst_rect.x", dstRect.x),
			cereal::make_nvp("dst_rect.y", dstRect.y),
			cereal::make_nvp("dst_rect.w", dstRect.w),
			cereal::make_nvp("dst_rect.h", dstRect.h)
		);
		if (typeid(archive) == typeid(cereal::JSONInputArchive)
			|| typeid(archive) == typeid(cereal::PortableBinaryInputArchive)) {
			this->setTextureAssetUrl(m_texture_asset.asset_url);
			scaleMode = static_cast<SDL_ScaleMode>(scale_mode);
			blendMode = static_cast<BlendMode>(blend_mode);
			flipMode = static_cast<FlipMode>(flip_mode);
			shape = static_cast<Shape>(shape_);
		}
	}
	//---------- Getter/Setter 实现 ----------
	std::string TextureComponent::getTextureAssetUrl() const {
		return m_texture_asset.asset_url; // 假设 Asset 类有 getUrl() 方法
	}

	void TextureComponent::setTextureAssetUrl(std::string url) {
		m_texture_asset = RuntimeGlobalContext::assetManager.loadAssetTexture(url);
		if (m_texture_asset.data) {
			SDL_GetTextureSize(static_cast<SDL_Texture*>(m_texture_asset.data),
				&texture_size.first, &texture_size.second);
			SDL_Log("Load Ok: %s,%f x %f", m_texture_asset.asset_url.c_str(), texture_size.first, texture_size.second);
		}
		else {
			texture_size.first = 0;
			texture_size.second = 0;
			SDL_Log("Load Error: %s", m_texture_asset.asset_url.c_str());
		}
		m_isDirty = true;
	}

	std::array<float, 2> GearX::TextureComponent::getTextureSize()
	{
		return std::array<float, 2>({texture_size.first,texture_size.second});
	}

	void GearX::TextureComponent::setColorMod(ColorType colort) {
		auto& color = colort.color;
		setRedMod(color[0]);
		setGreenMod(color[1]);
		setBlueMod(color[2]);
		setAlphaMod(color[3]);
	}

	ColorType GearX::TextureComponent::getColorMod() const {
		ColorType result;
		result.color = std::array<float, 4>{ redMod, greenMod, blueMod, alphaMod };
		return result;
	}

	// Alpha Mod
	float TextureComponent::getAlphaMod() const { return alphaMod; }
	void TextureComponent::setAlphaMod(float alpha) {
		if (alpha < 0.0f) {
			alpha = 0.0f;
		}
		else if (alpha > 1.0f) {
			alpha = 1.0f;
		}
		alphaMod = alpha;
		m_isDirty = true;
	}

	// Red Mod
	float TextureComponent::getRedMod() const { return redMod; }
	void TextureComponent::setRedMod(float red) {
		if (red < 0.0f) {
			red = 0.0f;
		}
		else if (red > 1.0f) {
			red = 1.0f;
		}
		redMod = red;
		m_isDirty = true;
	}

	// Green Mod
	float TextureComponent::getGreenMod() const { return greenMod; }
	void TextureComponent::setGreenMod(float green) {
		if (green < 0.0f) {
			green = 0.0f;
		}
		else if (green > 1.0f) {
			green = 1.0f;
		}
		greenMod = green;
		m_isDirty = true;
	}

	// Blue Mod
	float TextureComponent::getBlueMod() const { return blueMod; }
	void TextureComponent::setBlueMod(float blue) {
		if (blue < 0.0f) {
			blue = 0.0f;
		}
		else if (blue > 1.0f) {
			blue = 1.0f;
		}
		blueMod = blue;
		m_isDirty = true;
	}
	void GearX::TextureComponent::setSrcRect(std::array<float, 4> rect) {
		srcRect = { rect[0],rect[1],rect[2],rect[3] };
		m_isDirty = true;
	}
	std::array<float, 4> GearX::TextureComponent::getSrcRect() const
	{
		return { srcRect.x,srcRect.y,srcRect.w,srcRect.h };
	}

	void GearX::TextureComponent::setDstRect(std::array<float, 4> rect) {
		dstRect = { rect[0],rect[1],rect[2],rect[3] };

		auto parent = m_parentObject.lock();
		auto com = parent->getComponentByTypeName(rttr::type::get<TransformComponent>().get_name());
		auto t_com = std::dynamic_pointer_cast<TransformComponent>(com);
		if (t_com) {
			std::array<float, 2> scale = { 1.0f,1.0f };
			scale = t_com->getScale();
			//缩放
			dstRect.w = rect[2] / scale[0];
			dstRect.h = rect[3] / scale[1];
		}
		m_isDirty = true;
	}
	std::array<float, 4> GearX::TextureComponent::getDstRect() const {
		auto parent = m_parentObject.lock();
		auto com = parent->getComponentByTypeName(rttr::type::get<TransformComponent>().get_name());
		auto t_com = std::dynamic_pointer_cast<TransformComponent>(com);
		float w = dstRect.w;
		float h = dstRect.h;
		if (t_com) {
			std::array<float, 2> scale = { 1.0f,1.0f };
			scale = t_com->getScale();
			//缩放
			w *= scale[0];
			h *= scale[1];
		}
		return { dstRect.x ,dstRect.y ,w ,h };
	}
	// blend mode
	TextureComponent::BlendMode TextureComponent::getBlendMode() const { return blendMode; }
	void TextureComponent::setBlendMode(TextureComponent::BlendMode _blendMode) {
		this->blendMode = _blendMode;
		m_isDirty = true;
	}
	//flip mode
	TextureComponent::FlipMode TextureComponent::getFlipMode() const { return flipMode; }
	void TextureComponent::setFlipMode(TextureComponent::FlipMode _flipMode) {
		this->flipMode = _flipMode;
		m_isDirty = true;
	}
	SDL_ScaleMode TextureComponent::getScaleMode()const { return scaleMode; }
	void TextureComponent::setScaleMode(SDL_ScaleMode _scaleMode) {
		this->scaleMode = _scaleMode;
	}

	TextureComponent::Shape GearX::TextureComponent::getShape() const
	{
		return shape;
	}
	void GearX::TextureComponent::setShape(TextureComponent::Shape _shape)
	{
		this->shape = _shape;
	}
	float TextureComponent::getSrcRectX() const {
		return srcRect.x;
	}
	void TextureComponent::setSrcRectX(float x) {
		srcRect.x = x;
		m_isDirty = true;
	}
	float TextureComponent::getSrcRectY() const {
		return srcRect.y;
	}
	void TextureComponent::setSrcRectY(float y) {
		srcRect.y = y;
		m_isDirty = true;
	}
	float TextureComponent::getSrcRectW() const {
		return srcRect.w;
	}
	void TextureComponent::setSrcRectW(float w) {
		srcRect.w = w;
		m_isDirty = true;
	}
	float TextureComponent::getSrcRectH() const {
		return srcRect.h;
	}
	void TextureComponent::setSrcRectH(float h) {
		srcRect.h = h;
		m_isDirty = true;
	}

	float TextureComponent::getDstRectX() const {
		return dstRect.x;
	}
	void TextureComponent::setDstRectX(float x) {
		auto ptr = m_parentObject.lock();
		if (ptr) {
			auto com = ptr->getComponentByTypeName(rttr::type::get<TransformComponent>().get_name());
			if (com) {
				auto t_com = std::dynamic_pointer_cast<TransformComponent>(com);
				t_com->setPositionX(x);
			}
		}
		dstRect.x = x;
		m_isDirty = true;
	}
	float TextureComponent::getDstRectY() const {
		return dstRect.y;
	}
	void TextureComponent::setDstRectY(float y) {
		auto ptr = m_parentObject.lock();
		if (ptr) {
			auto com = ptr->getComponentByTypeName(rttr::type::get<TransformComponent>().get_name());
			if (com) {
				auto t_com = std::dynamic_pointer_cast<TransformComponent>(com);
				t_com->setPositionY(y);
			}
		}
		dstRect.y = y;
		m_isDirty = true;
	}
	float TextureComponent::getDstRectW() const {
		return dstRect.w;
	}
	void TextureComponent::setDstRectW(float w) {
		dstRect.w = w;
		m_isDirty = true;
	}
	float TextureComponent::getDstRectH() const {
		return dstRect.h;
	}
	void TextureComponent::setDstRectH(float h) {
		dstRect.h = h;
		m_isDirty = true;
	}
	Asset GearX::TextureComponent::getTextureAsset()
	{
		return m_texture_asset;
	}
} // namespace GearX
//using namespace GearX;
//CEREAL_REGISTER_DYNAMIC_INIT(TextureComponent)