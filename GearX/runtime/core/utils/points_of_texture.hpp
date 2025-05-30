#pragma once
#include "../../depencies.hpp"
namespace GearX {
    // ���ڸ��������ķ���ö��
    enum class Direction {
        RIGHT, DOWN, LEFT, UP
    };
    // �򻯰��Marching Squares�㷨ʵ��
    std::vector<b2Vec2> traceContour(Uint32* pixels, int width, int height, int threshold);
    // ��SDL������ȡ�㼯����������ײ��״
    std::vector<b2Vec2> getPointsFromTexture(SDL_Texture* texture);
}
