#include "points_of_texture.hpp"

// 简化版的Marching Squares算法实现
std::vector<b2Vec2> GearX::traceContour(Uint32* pixels, int width, int height, int threshold) {
    std::vector<b2Vec2> contour;
    std::vector<std::pair<int, int>> visited;

    // 找到起始点(第一个非透明像素)
    int startX = -1, startY = -1;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Uint8 alpha = (pixels[y * width + x] >> 24) & 0xFF;
            if (alpha > threshold) {
                startX = x;
                startY = y;
                goto found_start;
            }
        }
    }
found_start:

    if (startX == -1) return contour; // 没有找到非透明像素

    int x = startX, y = startY;
    Direction dir = Direction::RIGHT;
    int steps = 0;
    const int maxSteps = width * height * 4; // 防止无限循环

    do {
        // 检查当前点是否已访问
        auto it = std::find(visited.begin(), visited.end(), std::make_pair(x, y));
        if (it == visited.end()) {
            visited.push_back({ x, y });
            contour.push_back(b2Vec2(x / 30.0f, y / 30.0f)); // 转换为Box2D单位
        }

        // 根据当前方向检查相邻像素
        bool found = false;
        switch (dir) {
        case Direction::RIGHT:
            if (y > 0 && ((pixels[(y - 1) * width + x] >> 24) & 0xFF) <= threshold) {
                --y;
                dir = Direction::UP;
                found = true;
            }
            else if (x < width - 1 && ((pixels[y * width + (x + 1)] >> 24) & 0xFF) > threshold) {
                ++x;
                dir = Direction::RIGHT;
                found = true;
            }
            else if (y < height - 1 && ((pixels[(y + 1) * width + x] >> 24) & 0xFF) <= threshold) {
                ++y;
                dir = Direction::DOWN;
                found = true;
            }
            else {
                --x;
                dir = Direction::LEFT;
            }
            break;

        case Direction::DOWN:
            if (x < width - 1 && ((pixels[y * width + (x + 1)] >> 24) & 0xFF) <= threshold) {
                ++x;
                dir = Direction::RIGHT;
                found = true;
            }
            else if (y < height - 1 && ((pixels[(y + 1) * width + x] >> 24) & 0xFF) > threshold) {
                ++y;
                dir = Direction::DOWN;
                found = true;
            }
            else if (x > 0 && ((pixels[y * width + (x - 1)] >> 24) & 0xFF) <= threshold) {
                --x;
                dir = Direction::LEFT;
                found = true;
            }
            else {
                --y;
                dir = Direction::UP;
            }
            break;

        case Direction::LEFT:
            if (y < height - 1 && ((pixels[(y + 1) * width + x] >> 24) & 0xFF) <= threshold) {
                ++y;
                dir = Direction::DOWN;
                found = true;
            }
            else if (x > 0 && ((pixels[y * width + (x - 1)] >> 24) & 0xFF) > threshold) {
                --x;
                dir = Direction::LEFT;
                found = true;
            }
            else if (y > 0 && ((pixels[(y - 1) * width + x] >> 24) & 0xFF) <= threshold) {
                --y;
                dir = Direction::UP;
                found = true;
            }
            else {
                ++x;
                dir = Direction::RIGHT;
            }
            break;

        case Direction::UP:
            if (x > 0 && ((pixels[y * width + (x - 1)] >> 24) & 0xFF) <= threshold) {
                --x;
                dir = Direction::LEFT;
                found = true;
            }
            else if (y > 0 && ((pixels[(y - 1) * width + x] >> 24) & 0xFF) > threshold) {
                --y;
                dir = Direction::UP;
                found = true;
            }
            else if (x < width - 1 && ((pixels[y * width + (x + 1)] >> 24) & 0xFF) <= threshold) {
                ++x;
                dir = Direction::RIGHT;
                found = true;
            }
            else {
                ++y;
                dir = Direction::DOWN;
            }
            break;
        }

        steps++;
    } while ((x != startX || y != startY) && steps < maxSteps);

    // 简化轮廓点(减少点数量同时保持形状)
    std::vector<b2Vec2> simplified;
    if (contour.size() > 2) {
        simplified.push_back(contour[0]);

        for (size_t i = 1; i < contour.size() - 1; ++i) {
            // 计算三点形成的角度
            b2Vec2 v1 = contour[i] - contour[i - 1];
            b2Vec2 v2 = contour[i + 1] - contour[i];
            float dot = b2Dot(v1, v2);
            float cross = v1.x * v2.y - v1.y * v2.x;
            float angle = std::atan2(cross, dot) * 180.0f / b2_pi;

            // 如果角度变化足够大，则保留该点
            if (std::abs(angle) > 15.0f) { // 角度阈值可调整
                simplified.push_back(contour[i]);
            }
        }

        simplified.push_back(contour.back());
    }
    else {
        simplified = std::move(contour);
    }

    return simplified;
}

// 从SDL纹理提取点集用于物理碰撞形状
std::vector<b2Vec2> GearX::getPointsFromTexture(SDL_Texture* texture) {
    std::vector<b2Vec2> points;
    if (!texture) return points;

    // 获取纹理信息
    float width, height;
    SDL_GetTextureSize(texture, &width, &height);
    SDL_PixelFormat format = texture->format;
    // 创建临时表面用于像素访问
    
    SDL_Surface* surface = SDL_CreateSurface(width, height, format);

    if (!surface) return points;
    // 将纹理渲染到表面
    SDL_Renderer* tempRenderer = SDL_CreateSoftwareRenderer(surface);
    SDL_RenderTexture(tempRenderer, texture, nullptr, nullptr);
    SDL_RenderPresent(tempRenderer);

    // 锁定表面以访问像素
    if (!SDL_LockSurface(surface)) {
        SDL_DestroySurface(surface);
        SDL_DestroyRenderer(tempRenderer);
        return points;
    }
    // 获取像素数据
    Uint32* pixels = static_cast<Uint32*>(surface->pixels);

    // 使用改进的轮廓跟踪算法
    points = traceContour(pixels, width, height, 128); // 透明度阈值设为128

    // 解锁表面
    SDL_UnlockSurface(surface);

    // 清理资源
    SDL_DestroySurface(surface);
    SDL_DestroyRenderer(tempRenderer);

    return points;
}