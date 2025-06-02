#pragma once
#include "../component.hpp"
namespace GearX {
	class Asset;
	// Texture Component
	class Component;
	struct ColorType {
	public:
		std::array<float, 4> color;
		RTTR_ENABLE();
	};
	class TextureComponent : public Component {
	private:
		RTTR_ENABLE(Component) // 标记继承链
			RTTR_REGISTRATION_FRIEND // 允许 RTTR 访问私有成员
	public:
		//  BlendMode 枚举
		enum class BlendMode : Uint8 {
			None = SDL_BLENDMODE_NONE,
			Blend = SDL_BLENDMODE_BLEND,
			Add = SDL_BLENDMODE_ADD,
			Mod = SDL_BLENDMODE_MOD,
			AddPremultiplied = SDL_BLENDMODE_ADD_PREMULTIPLIED,
			BlendPremultiplied = SDL_BLENDMODE_BLEND_PREMULTIPLIED,
		};
		enum class FlipMode : Uint8 {
			None = SDL_FLIP_NONE,
			Horizontal = SDL_FLIP_HORIZONTAL,
			Vertical = SDL_FLIP_VERTICAL,
			VerticalAndHorizontal = SDL_FLIP_VERTICAL | SDL_FLIP_HORIZONTAL
		};
		enum class Shape : Uint8 {
			Rect = 0,
			Circle = 1,
			Polygon = 2
		};
		// Mod
		float alphaMod = 1.0f;
		float redMod = 1.0f;
		float greenMod = 1.0f;
		float blueMod = 1.0f;
		SDL_ScaleMode scaleMode = SDL_SCALEMODE_LINEAR;
		BlendMode blendMode = BlendMode::None;
		FlipMode flipMode = FlipMode::None;
		Shape shape = Shape::Rect;
		float angle = 0.0f;
		SDL_FPoint orgin = { 256.0f,256.0f };
		// Texture Render Configs
		SDL_FRect srcRect;
		SDL_FRect dstRect;
		Asset m_texture_asset;
		std::pair<float, float> texture_size;
		// 当形状为多边形时，存储顶点坐标 -- 未使用
		std::vector<std::array<float, 2>> vertices;
		bool isTextureStateChanged = false;
		friend class RenderSystem;
	public:
		TextureComponent() {
			m_type_name = "TextureComponent";
			m_texture_asset = Asset();
			srcRect = { 0.0f,0.0f,512.0f,512.0f };
			dstRect = { 0.0f,0.0f,512.0f,512.0f };
			texture_size.first = 0;
			texture_size.second = 0;
		}
		//RTTR 反射调用
		static void addComponentTo(std::shared_ptr<GObject> obj);
		std::shared_ptr<Component> copy();
		//Cereal 序列化支持
		template<class Archive>
		void serialize(Archive& archive);
		// Getter and Setter
		std::string getTextureAssetUrl() const;
		void setTextureAssetUrl(std::string url);
		std::array<float, 2> getTextureSize();
		void setColorMod(ColorType);
		ColorType getColorMod()const;
		//Color
		float getAlphaMod() const;
		void setAlphaMod(float alpha);
		float getRedMod() const;
		void setRedMod(float red);
		float getGreenMod() const;
		void setGreenMod(float green);
		float getBlueMod() const;
		void setBlueMod(float blue);
		// SrcRect
		void setSrcRect(std::array<float, 4>);
		std::array<float, 4> getSrcRect()const;

		float getSrcRectX() const;
		void setSrcRectX(float x);
		float getSrcRectY() const;
		void setSrcRectY(float y);
		float getSrcRectW() const;
		void setSrcRectW(float w);
		float getSrcRectH() const;
		void setSrcRectH(float h);

		void setDstRect(std::array<float, 4>);
		std::array<float, 4> getDstRect()const;
		float getDstRectX() const;
		void setDstRectX(float x);
		float getDstRectY() const;
		void setDstRectY(float y);
		float getDstRectW() const;
		void setDstRectW(float w);
		float getDstRectH() const;
		void setDstRectH(float h);
		void setTextureChangaed(bool _isTextureChangable) {
			isTextureStateChanged = _isTextureChangable;
		}
		bool isTextureChanged() const{return isTextureStateChanged;}
		void setDstSize(std::array<float, 2> size);
		std::array<float, 2> getDstSize();
		Asset getTextureAsset();
		BlendMode getBlendMode() const;
		void setBlendMode(BlendMode _blendMode);
		FlipMode getFlipMode() const;
		void setFlipMode(FlipMode _flipMode);
		SDL_ScaleMode getScaleMode()const;
		void setScaleMode(SDL_ScaleMode _scaleMode);
		Shape getShape() const;
		void setShape(Shape _shape);
	};
}
CEREAL_REGISTER_TYPE(GearX::TextureComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(GearX::Component, GearX::TextureComponent)