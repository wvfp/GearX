#pragma once
#include "../../depencies.hpp"
namespace GearX {
    // 用于跟踪轮廓的方向枚举
    enum class Direction {
        RIGHT, DOWN, LEFT, UP
    };
    // 简化版的Marching Squares算法实现
    std::vector<b2Vec2> traceContour(Uint32* pixels, int width, int height, int threshold);
    // 从SDL纹理提取点集用于物理碰撞形状
    std::vector<b2Vec2> getPointsFromTexture(SDL_Texture* texture);
}
