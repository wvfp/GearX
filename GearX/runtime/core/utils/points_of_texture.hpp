#pragma once
#include "../../depencies.hpp"
namespace GearX {
    // ��SDL������ȡ�㼯����������ײ��״,�����������ʹ��
    b2PolygonShape getBox2DShape(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect* srcRect,
        SDL_FRect* dstRect,float scaleX = 1.0f,float scaleY = 1.0f, bool flipX = false, bool flipY = false);
    // ��SDL������ȡһ��������������ײ��״,�����������������ײ��⣬������͸������
    SDL_Rect getBoxRect(SDL_Renderer* renderer, SDL_Texture* texture,
        SDL_Rect srcRect);
}
