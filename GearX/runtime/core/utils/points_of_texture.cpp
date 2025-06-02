#include "points_of_texture.hpp"
#include "../../depencies.hpp"
constexpr int TARGET_SIZE = 64;
#define sample  TARGET_SIZE/64 // 采样间隔
using namespace std;
SDL_Surface* convertToSurface(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect* rect = nullptr) {
    // 参数有效性检查
    if (!renderer || !texture) {
        return nullptr;
    }

    static SDL_Texture* renderTarget = nullptr;

    // 初始化渲染目标纹理
    if (!renderTarget) {
        renderTarget = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            TARGET_SIZE,
            TARGET_SIZE
        );
        if (!renderTarget) {
            return nullptr;
        }
        SDL_SetTextureScaleMode(renderTarget, SDL_SCALEMODE_NEAREST);
        SDL_SetTextureBlendMode(renderTarget, SDL_BLENDMODE_BLEND);
    }
  
    // 获取源纹理尺寸
    float srcWidth, srcHeight;
    if (!SDL_GetTextureSize(texture, &srcWidth, &srcHeight)) {
        return nullptr;
    }

    // 计算实际渲染尺寸
    const int w = rect ? rect->w : srcWidth;
    const int h = rect ? rect->h : srcHeight;
    if (w <= 0 || h <= 0) {
        return nullptr;
    }
    // 保持纵横比的缩放计算
    const float aspectRatio = static_cast<float>(w) / h;
    const int scaledWidth = static_cast<int>(TARGET_SIZE * aspectRatio);
    const int scaledHeight = TARGET_SIZE;
    const int offsetX = (TARGET_SIZE - scaledWidth) / 2;
    // 设置渲染目标
    SDL_SetRenderTarget(renderer, renderTarget);

    // 清空渲染目标
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // 设置源纹理区域
    SDL_FRect srcRect = { 0, 0, static_cast<float>(w), static_cast<float>(h)};
    if (rect) {
        srcRect.x = rect->x;
        srcRect.y = rect->y;
        srcRect.w = rect->w;
        srcRect.h = rect->h;
    }
    SDL_FRect dstRect = { offsetX, 0, (float)scaledWidth, (float)scaledHeight };
    // 执行渲染
    if (!SDL_RenderTexture(renderer, texture, &srcRect, &dstRect)) {
        SDL_SetRenderTarget(renderer, nullptr);
        return nullptr;
    }

    SDL_Surface* surface = SDL_RenderReadPixels(renderer, nullptr);

    // 恢复默认渲染目标
    SDL_SetRenderTarget(renderer, nullptr);

    return surface;
}

struct Point {
    float x, y;
    Point(float x_ = 0, float y_ = 0) : x(x_), y(y_) {}

    bool operator<(const Point& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }
};

static inline float cross(const Point& a, const Point& b) {
    return a.x * b.y - a.y * b.x;
}
// 计算两点之间的欧氏距离
float distance(const Point& a, const Point& b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}
// 计算三角形面积的两倍（用于判断点的偏离程度）
float triangleArea(const Point& a, const Point& b, const Point& c) {
    return std::abs((a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)));
}

std::vector<Point> limit_vertices(const std::vector<Point>& hull, int max_v) {
    if (hull.size() <= max_v) return hull;
    if (max_v < 3) return {}; // 至少需要3个点形成多边形

    // 使用Ramer-Douglas-Peucker算法简化多边形
    std::vector<Point> result = hull;

    while (result.size() > max_v) {
        float minArea = FLT_MAX;
        int removeIdx = -1;

        // 找到可以删除的最不重要的点（即对形状影响最小的点）
        for (size_t i = 0; i < result.size(); ++i) {
            size_t prev = (i == 0) ? result.size() - 1 : i - 1;
            size_t next = (i == result.size() - 1) ? 0 : i + 1;

            float area = triangleArea(result[prev], result[i], result[next]);

            if (area < minArea) {
                minArea = area;
                removeIdx = i;
            }
        }

        // 删除最不重要的点
        if (removeIdx != -1) {
            result.erase(result.begin() + removeIdx);
        }
        else {
            break; // 防止无限循环
        }
    }

    return result;
}
// 凸包算法（Andrew's monotone chain）
vector<Point> convex_hull(vector<Point> points) {
    int n = points.size();
    if (n <= 1) return points;

    sort(points.begin(), points.end());

    vector<Point> hull;
    // 下凸壳
    for (int i = 0; i < n; hull.push_back(points[i++]))
        while (hull.size() >= 2 && cross(hull.back() - hull[hull.size() - 2], points[i] - hull.back()) <= 0)
            hull.pop_back();

    // 上凸壳
    for (int i = n - 2, t = hull.size(); i >= 0; hull.push_back(points[i--]))
        while (hull.size() > t && cross(hull.back() - hull[hull.size() - 2], points[i] - hull.back()) <= 0)
            hull.pop_back();

    hull.pop_back(); // 移除最后一个重复点
    return hull;
}

vector<Point> get_polygon(SDL_Surface* surface, float scale = 1.0f) {
    vector<Point> points;

    if (!surface || !surface->pixels) return {};

    // 获取像素格式参数
    const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(surface->format);
    const SDL_Palette* palette = SDL_GetSurfacePalette(surface);
    const int pitch = surface->pitch;
    Uint32* pixel = (Uint32*)surface->pixels;
    // 遍历所有像素（带下采样）
    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 128);
    for (int y = 0; y < surface->h; y += sample) {
        for (int x = 0; x < surface->w; x += sample) {
            Uint8 alpha;
            SDL_GetRGBA(pixel[y * surface->w + x], format, palette,
                nullptr, nullptr, nullptr, &alpha);
            if (alpha >= 128) {
                points.emplace_back(Point(x, y));
                SDL_RenderPoint(renderer, x, y);
            }
        }
    }
    SDL_RenderPresent(renderer);
    SDL_DestroyRenderer(renderer);
    if (points.empty()) return {};

    // 计算凸包
    auto hull = convex_hull(points);

    if (hull.size() < 3) return {};
    return hull;
}


// 计算多边形的几何中心（质心）
Point computeCentroid(const std::vector<Point>& vertices) {
    float cx = 0.0f, cy = 0.0f;
    float area = 0.0f;
    const float factor = 1.0f / 3.0f;

    for (size_t i = 0; i < vertices.size(); ++i) {
        size_t j = (i + 1) % vertices.size();
        float x_i = vertices[i].x;
        float y_i = vertices[i].y;
        float x_j = vertices[j].x;
        float y_j = vertices[j].y;

        float cross = x_i * y_j - x_j * y_i;
        area += cross;

        cx += (x_i + x_j) * cross;
        cy += (y_i + y_j) * cross;
    }

    if (area == 0.0f) {
        // 处理退化情况（所有点共线）
        return Point(0, 0);
    }

    area *= 0.5f;
    float invDenom = 1.0f / (6.0f * area);

    return Point(cx * invDenom, cy * invDenom);
}

// 将多边形顶点转换为相对于几何中心的坐标系
std::vector<Point> convertToCentroidFrame(const std::vector<Point>& vertices) {
    Point centroid = computeCentroid(vertices);
    std::vector<Point> result;
    result.reserve(vertices.size());

    for (const auto& vertex : vertices) {
        // 每个顶点减去几何中心坐标
        result.emplace_back(vertex.x - centroid.x, vertex.y - centroid.y);
    }

    return result;
}
template<typename T>
void RenderVertices(SDL_Renderer* renderer, const std::vector<T>& vertices, const SDL_Color& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int i = 0; i < vertices.size() - 1; i++) {
        SDL_RenderLine(renderer, vertices[i].x, vertices[i].y, vertices[i + 1].x, vertices[i + 1].y);
    }
    SDL_RenderLine(renderer, vertices.front().x, vertices.front().y, vertices.back().x, vertices.back().y);
    std::cout << "\nRenderVertices :" << vertices.size() << std::endl;
    for (auto& v : vertices) {
        std::cout << "\nVertex ";
        std::cout << "  " << v.x << " " << v.y << "\t";

    }
}
std::map<SDL_Texture*, SDL_Surface*> surfaces;
b2PolygonShape GearX::getBox2DShape(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect* rect, SDL_FRect *dstRect,
    float scaleX,float scaleY,bool flipX, bool flipY) {
    b2PolygonShape shape;
    shape.SetAsBox(1, 1); // 占位，待计算
    if (!texture && !renderer) {
        return shape;
    }
    // 获取纹理对应的SDL_Surface,纹理固定大小TEARGET_SIZE*TARGET_SIZE,
    // 纹理将平铺到目标大小
    if (surfaces.find(texture) == surfaces.end()) {
        surfaces[texture] = convertToSurface(renderer, texture, rect);
    }
	// 计算Box2D坐标
    // 计算顶点坐标
    // 顶点坐标限制到一个TARGET_SIZE*TARGET_SIZE的矩形内
    std::vector<Point> points = get_polygon(surfaces[texture]);
    points = limit_vertices(points, b2_maxPolygonVertices);
    // 转换为相对于几何中心的坐标系
    points = convertToCentroidFrame(points);
    if (points.size() < 3)
        return shape;
	auto rdr = SDL_CreateSoftwareRenderer(surfaces[texture]);
	RenderVertices(rdr, points, { 255, 0, 0, 255 });
	SDL_RenderPresent(rdr);
	IMG_SavePNG(surfaces[texture], "test.png");
	SDL_Log("save png");
    float scale = 1.0f;
    // 将顶点缩放到(-1.0f,1.0f)的缩放
    scale /= TARGET_SIZE;
    // 计算像素->米的转换
    scale /= PPM;
    // 将顶点缩放到dstRect的尺寸,dstRect->w/2,dstRect->h/2为中心点
    scaleX *= scale * dstRect->w / 2;
    scaleY *= scale * dstRect->h / 2;
    // 计算缩放后的顶点坐标
    std::vector<b2Vec2> b2points;
    // 转换为Box2D坐标并标准化
    for (const auto& p : points) {
        b2Vec2 point = b2Vec2(p.x * scaleX, p.y * scaleY);
        if (flipX) {
            point.x =  - point.x;
        }
        if (flipY) {
            point.y =  - point.y;
        }
        b2points.emplace_back(point);
    }
    // 
    shape.Set(b2points.data(), b2points.size());
    return shape;
}

// 获取图像中的可以包围其非透明区域的最小矩形
// 返回值：最小矩形
SDL_Rect GearX::getBoxRect(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect srcRect) {
    SDL_Rect rect = srcRect;  // 默认返回原始矩形

    // 检查无效参数
    if (!texture || !renderer) {
        return rect;
    }
    SDL_Surface* surface = convertToSurface(renderer, texture, &srcRect);
    if (!surface) {
        return  rect;  // 创建失败返回矩形
    }

    // 初始化边界值
    int minX = surface->w;
    int maxX = -1;
    int minY = surface->h;
    int maxY = -1;
    bool foundOpaque = false;

    // 锁定表面以访问像素
    if (SDL_LockSurface(surface)) {
        Uint32* pixel = (Uint32*)surface->pixels;
        const SDL_PixelFormatDetails* formate = SDL_GetPixelFormatDetails(surface->format);
        const SDL_Palette* palette = SDL_GetSurfacePalette(surface);
        // 遍历所有像素
        for (int y = 0; y < surface->h; y++) {
            for (int x = 0; x < surface->w; x++) {
                Uint32 pixelValue = pixel[y * surface->w + x];
                Uint8 alpha;
                // 获取Alpha值
				SDL_GetRGBA(pixelValue,formate, palette, NULL, NULL, NULL, &alpha);
                // 检查Alpha值
                if (alpha >= 128) {
                    foundOpaque = true;
                    if (x < minX) minX = x;
                    if (x > maxX) maxX = x;
                    if (y < minY) minY = y;
                    if (y > maxY) maxY = y;
                }
            }
        }

        SDL_UnlockSurface(surface);
        SDL_DestroySurface(surface);
    }
    else {
        return srcRect;
    }

    // 处理边界情况
    if (!foundOpaque) {
        return srcRect;
    }
    float scaleX = (float)srcRect.w / (float)surface->w;
    float scaleY = (float)srcRect.h / (float)surface->h;
    // 返回最小包围矩形
    return {
        static_cast<int>(minX*scaleX),
        static_cast<int>(minY*scaleY),
        static_cast<int>(maxX*scaleX - minX*scaleX),
        static_cast<int>(maxY*scaleY - minY*scaleY)
    };
}
