#include "points_of_texture.hpp"

// �򻯰��Marching Squares�㷨ʵ��
std::vector<b2Vec2> GearX::traceContour(Uint32* pixels, int width, int height, int threshold) {
    std::vector<b2Vec2> contour;
    std::vector<std::pair<int, int>> visited;

    // �ҵ���ʼ��(��һ����͸������)
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

    if (startX == -1) return contour; // û���ҵ���͸������

    int x = startX, y = startY;
    Direction dir = Direction::RIGHT;
    int steps = 0;
    const int maxSteps = width * height * 4; // ��ֹ����ѭ��

    do {
        // ��鵱ǰ���Ƿ��ѷ���
        auto it = std::find(visited.begin(), visited.end(), std::make_pair(x, y));
        if (it == visited.end()) {
            visited.push_back({ x, y });
            contour.push_back(b2Vec2(x / 30.0f, y / 30.0f)); // ת��ΪBox2D��λ
        }

        // ���ݵ�ǰ��������������
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

    // ��������(���ٵ�����ͬʱ������״)
    std::vector<b2Vec2> simplified;
    if (contour.size() > 2) {
        simplified.push_back(contour[0]);

        for (size_t i = 1; i < contour.size() - 1; ++i) {
            // ���������γɵĽǶ�
            b2Vec2 v1 = contour[i] - contour[i - 1];
            b2Vec2 v2 = contour[i + 1] - contour[i];
            float dot = b2Dot(v1, v2);
            float cross = v1.x * v2.y - v1.y * v2.x;
            float angle = std::atan2(cross, dot) * 180.0f / b2_pi;

            // ����Ƕȱ仯�㹻�������õ�
            if (std::abs(angle) > 15.0f) { // �Ƕ���ֵ�ɵ���
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

// ��SDL������ȡ�㼯����������ײ��״
std::vector<b2Vec2> GearX::getPointsFromTexture(SDL_Texture* texture) {
    std::vector<b2Vec2> points;
    if (!texture) return points;

    // ��ȡ������Ϣ
    float width, height;
    SDL_GetTextureSize(texture, &width, &height);
    SDL_PixelFormat format = texture->format;
    // ������ʱ�����������ط���
    
    SDL_Surface* surface = SDL_CreateSurface(width, height, format);

    if (!surface) return points;
    // ��������Ⱦ������
    SDL_Renderer* tempRenderer = SDL_CreateSoftwareRenderer(surface);
    SDL_RenderTexture(tempRenderer, texture, nullptr, nullptr);
    SDL_RenderPresent(tempRenderer);

    // ���������Է�������
    if (!SDL_LockSurface(surface)) {
        SDL_DestroySurface(surface);
        SDL_DestroyRenderer(tempRenderer);
        return points;
    }
    // ��ȡ��������
    Uint32* pixels = static_cast<Uint32*>(surface->pixels);

    // ʹ�øĽ������������㷨
    points = traceContour(pixels, width, height, 128); // ͸������ֵ��Ϊ128

    // ��������
    SDL_UnlockSurface(surface);

    // ������Դ
    SDL_DestroySurface(surface);
    SDL_DestroyRenderer(tempRenderer);

    return points;
}