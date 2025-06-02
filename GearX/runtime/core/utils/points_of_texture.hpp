#pragma once
#include "../../depencies.hpp"
namespace GearX {
    // 从SDL纹理提取点集用于物理碰撞形状,这个函数不能使用
    b2PolygonShape getBox2DShape(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect* srcRect,
        SDL_FRect* dstRect,float scaleX = 1.0f,float scaleY = 1.0f, bool flipX = false, bool flipY = false);
    // 从SDL纹理提取一个框用于物理碰撞形状,这个函数可以用于碰撞检测，忽略了透明像素
    SDL_Rect getBoxRect(SDL_Renderer* renderer, SDL_Texture* texture,
        SDL_Rect srcRect);
}
