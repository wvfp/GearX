#pragma once
#include "../global/global.hpp"
#include "../framework/component/texture/texture_component.hpp"
// ��������ò���
#define AA_SAMPLES 4        // ��������
#define AA_SCALE 2.0f       // ��Ⱦ�ֱ�����������
namespace GearX {
	void drawRect(SDL_Renderer* renderer, SDL_Texture* texture,
		const SDL_FRect* srcRect, const SDL_FRect* dst_rect,
		float angle, SDL_FPoint* center, TextureComponent::FlipMode flipmode);
	void drawCircle(SDL_Renderer* renderer, SDL_Texture* texture,
		const SDL_FRect* srcRect, const SDL_FRect* dst_rect,
		float angle, SDL_FPoint* center, TextureComponent::FlipMode flipmode);
}