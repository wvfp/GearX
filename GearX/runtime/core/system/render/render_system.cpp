#include "render_system.hpp"
#include "../../global/global.hpp"
#include "../../framework/component/transform/transform_component.hpp"
#include "../../framework/component/texture/texture_component.hpp"
#include "../../framework/component/rigidbody/rigidbody_component.hpp"
#include "../../utils/render_axis.hpp"
#include "../../utils/render_shape.hpp"

void GearX::RenderSystem::tick(float deltaTime) {
	if (!RuntimeGlobalContext::world.getCurrentLevel())
		return;
	else {
		auto& objs = RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
		auto& layers = RuntimeGlobalContext::world.getCurrentLevel()->getLayers();
		//按照层来遍历对象，保证绘制顺序
		for (auto& layer : layers) {
			for (auto id : layer) {
				auto obj = objs[id];

				std::array<float, 2> scale = { 1.0f,1.0f };
				auto com1 = obj->getComponentByTypeName(rttr::type::get<TransformComponent>().get_name());
				auto com2 = obj->getComponentByTypeName(rttr::type::get<TextureComponent>().get_name());
				auto com3 = obj->getComponentByTypeName(rttr::type::get<RigidBodyComponent>().get_name());
				auto transform_com = std::dynamic_pointer_cast<TransformComponent>(com1);
				auto texture_com = std::dynamic_pointer_cast<TextureComponent>(com2);
				auto rigidbody_com = std::dynamic_pointer_cast<RigidBodyComponent>(com3);
				if (!texture_com)
					continue;
				SDL_FRect dstRect = { 0,0,0,0 };
				SDL_FRect s_dstRect = { 0,0,0,0 };
				SDL_FPoint orgin = { 0,0 };
				Asset asset = texture_com->getTextureAsset();
				float angle;
				auto renderer = RuntimeGlobalContext::SDL_CONTEXT.renderer;
				auto r_texture = RuntimeGlobalContext::SDL_CONTEXT.texture;
				auto texture = static_cast<SDL_Texture*>(asset.data);
				auto p_tex = SDL_GetRenderTarget(renderer);
				// Texture 属性获取
				float alpha, red, green, blue;
				SDL_BlendMode bm;
				SDL_ScaleMode s_mode;
				{   // 获得改变前的属性
					SDL_GetTextureBlendMode(texture, &bm);
					SDL_GetTextureAlphaModFloat(texture, &alpha);
					SDL_GetTextureColorModFloat(texture, &red, &green, &blue);
					SDL_GetTextureScaleMode(texture, &s_mode);
				}
				if (obj) {
					dstRect = texture_com->dstRect;

					int S = 8;
					dstRect.x *= S;
					dstRect.y *= S;
					dstRect.w *= S;
					dstRect.h *= S;
					orgin = texture_com->orgin;
					orgin.x *= S;
					orgin.y *= S;
					angle = texture_com->angle;
					//缩放
					if (transform_com)
						scale = transform_com->getScale();
					s_dstRect = dstRect;
					s_dstRect.w *= scale[0];
					s_dstRect.h *= scale[1];
					if (texture) {
						// 设置渲染目标，设置Texture参数，渲染Texture
						SDL_SetRenderTarget(renderer, r_texture);
						SDL_SetTextureBlendMode(texture, (SDL_BlendMode)texture_com->blendMode);
						SDL_SetTextureAlphaModFloat(texture, texture_com->alphaMod);
						SDL_SetTextureColorModFloat(texture, texture_com->redMod, texture_com->greenMod, texture_com->blueMod);
						SDL_SetTextureScaleMode(texture, texture_com->scaleMode);
						if (texture_com->shape == TextureComponent::Shape::Rect) {
							SDL_RenderTextureRotated(renderer, texture,
								&texture_com->srcRect, &s_dstRect, angle, &orgin, SDL_FlipMode(texture_com->flipMode));
						}
						else if (texture_com->shape == TextureComponent::Shape::Circle) {
							drawCircle(renderer, texture,
								&texture_com->srcRect, &s_dstRect, angle, &orgin, texture_com->flipMode);
							orgin = { s_dstRect.w / 2, s_dstRect.h / 2 };
						}
						if (RuntimeGlobalContext::isGameMode) {
							if (rigidbody_com) {
								b2Fixture* fixture = rigidbody_com->getFixture();
								if (fixture) {
									//static int num = 0;
									//num++;
									//if (num % 20 == 0) {
									//	auto aabb = fixture->GetAABB(0);
									//	SDL_FRect rect = { aabb.lowerBound.x, aabb.lowerBound.y,
									//		aabb.upperBound.x - aabb.lowerBound.x, aabb.upperBound.y - aabb.lowerBound.y };
									//	SDL_Log("\nID:%d\n", obj->getID());
									//	SDL_Log("\nX:%f,Y:%f", rect.x*PPM, rect.y*PPM);
									//	SDL_Log("\nX:%f,Y:%f", (rect.x- rect.w)*PPM,(rect.y- rect.h)*PPM);
									//}
									////SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
									////SDL_RenderFillRect(renderer, &s_dstRect);
								}
							}
						}
					}
					{// 还原属性
						SDL_SetTextureBlendMode(texture, bm);
						SDL_SetTextureAlphaModFloat(texture, alpha);
						SDL_SetTextureColorModFloat(texture, red, green, blue);
						SDL_SetTextureScaleMode(texture, s_mode);
						
						if (!RuntimeGlobalContext::isGameMode&&obj->isDrawAxis())
							drawObjectAxes(renderer, s_dstRect, angle, orgin, 20, 10);
					
						SDL_SetRenderTarget(renderer, p_tex);
					}
				}
			}
		}
	}
}