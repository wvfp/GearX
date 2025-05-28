#pragma once
#include "../global/global.hpp"
#include "../framework/component/texture/texture_component.hpp"
// 抗锯齿配置参数
#define AA_SAMPLES 4        // 采样级别
#define AA_SCALE 2.0f       // 渲染分辨率缩放因子
namespace GearX {
	void drawRect(SDL_Renderer* renderer, SDL_Texture* texture,
		const SDL_FRect* srcRect, const SDL_FRect* dst_rect,
		float angle, SDL_FPoint* center, TextureComponent::FlipMode flipmode);
	void drawCircle(SDL_Renderer* renderer, SDL_Texture* texture,
		const SDL_FRect* srcRect, const SDL_FRect* dst_rect,
		float angle, SDL_FPoint* center, TextureComponent::FlipMode flipmode);
}