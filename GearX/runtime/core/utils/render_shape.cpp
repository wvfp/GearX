#pragma once
#include "render_shape.hpp"
namespace GearX {
	/*  *
		* @brief 使用SDL_RenderGeometry实现纹理旋转渲染
		* @param renderer 渲染器指针
		* @param texture 待渲染的纹理
		* @param dst_rect 目标矩形（位置和尺寸）
		* @param angle 旋转角度（度，顺时针为正）
		* @param center 旋转中心点（NULL表示目标矩形中心）
		*/
	static SDL_Texture* Target_Texture = nullptr;
	void drawRect(SDL_Renderer* renderer, SDL_Texture* texture,
		const SDL_FRect* srcRect, const SDL_FRect* dst_rect,
		float angle, SDL_FPoint* center, TextureComponent::FlipMode flipmode) {
		// 获取纹理尺寸
		float texWidth, texHeight;
		SDL_GetTextureSize(texture, &texWidth, &texHeight);
		// 计算旋转中心
		SDL_FPoint pivot;
		if (center) {
			pivot.x = dst_rect->x + center->x;
			pivot.y = dst_rect->y + center->y;
		}
		else {
			pivot.x = dst_rect->x + dst_rect->w / 2;
			pivot.y = dst_rect->y + dst_rect->h / 2;
		}

		// 计算纹理坐标（归一化）
		SDL_FRect src = srcRect ? *srcRect : SDL_FRect({ 0, 0, (float)texWidth, (float)texHeight });
		// 处理翻转模式
		SDL_FRect flippedSrc = src;
		if (flipmode == TextureComponent::FlipMode::None) {
			flippedSrc = src;
		}
		else if (flipmode == TextureComponent::FlipMode::Horizontal) {
			flippedSrc.x = src.x + src.w;
			flippedSrc.w = -src.w;
		}
		else if (flipmode == TextureComponent::FlipMode::Vertical) {
			flippedSrc.y = src.y + src.h;
			flippedSrc.h = -src.h;
		}
		else if (flipmode == TextureComponent::FlipMode::VerticalAndHorizontal) {
			flippedSrc.x = src.x + src.w;
			flippedSrc.w = -src.w;
			flippedSrc.y = src.y + src.h;
			flippedSrc.h = -src.h;
		}
		// 计算归一化UV坐标
		SDL_FPoint uv[4] = {
			{ (float)flippedSrc.x / texWidth, (float)flippedSrc.y / texHeight },
			{ (float)(flippedSrc.x + flippedSrc.w) / texWidth, (float)flippedSrc.y / texHeight },
			{ (float)(flippedSrc.x + flippedSrc.w) / texWidth, (float)(flippedSrc.y + flippedSrc.h) / texHeight },
			{ (float)flippedSrc.x / texWidth, (float)(flippedSrc.y + flippedSrc.h) / texHeight }
		};
		// 定义基础顶点（局部坐标）
		const SDL_FPoint basePoints[] = {
			{0, 0},                   // 左上
			{dst_rect->w, 0},         // 右上
			{dst_rect->w, dst_rect->h},// 右下
			{0, dst_rect->h}          // 左下
		};

		// 旋转变换参数
		const double radians = angle * M_PI / 180.0;
		const float cos_theta = (float)cos(radians);
		const float sin_theta = (float)sin(radians);

		// 构建顶点数据
		SDL_Vertex vertices[4];
		for (int i = 0; i < 4; ++i) {
			// 转换为旋转中心相对坐标
			float dx = basePoints[i].x - (pivot.x - dst_rect->x);
			float dy = basePoints[i].y - (pivot.y - dst_rect->y);

			// 应用旋转矩阵
			vertices[i].position.x = pivot.x + dx * cos_theta - dy * sin_theta;
			vertices[i].position.y = pivot.y + dx * sin_theta + dy * cos_theta;

			// 设置纹理坐标和颜色
			vertices[i].tex_coord = uv[i];
			SDL_FColor color = SDL_FColor({ 1.0f,1.0f, 1.0f, 1.0f });
			SDL_GetTextureAlphaModFloat(texture, &color.a);
			SDL_GetTextureColorModFloat(texture, &color.r, &color.g, &color.b);
			vertices[i].color = color;
		}

		// 定义索引（两个三角形组成矩形）
		const int indices[] = { 0, 1, 2, 0, 2, 3 };
	}
	void drawCircle(SDL_Renderer* renderer, SDL_Texture* texture,
		const SDL_FRect* srcRect, const SDL_FRect* dst_rect,
		float angle, SDL_FPoint* center, TextureComponent::FlipMode flipmode) {
		const float radius = dst_rect->w * 0.5f;  // 圆半径（假设宽高相等）
		const int segments = 100;                 // 分段数
		const float angleRad = angle * M_PI / 180.0f;
		const float angleStep = 2.0f * M_PI / segments;
		// 旋转矩阵参数
		const float cos_theta = cosf(angleRad);
		const float sin_theta = sinf(angleRad);

		// 计算圆心
		SDL_FPoint centerPos = {
			dst_rect->x + (center ? center->x : radius),
			dst_rect->y + (center ? center->y : radius)
		};

		// 顶点数据
		SDL_Vertex vertices[segments + 1];

		for (int i = 0; i <= segments; ++i) {
			// 标准圆形参数方程
			float theta = 2.0f * M_PI * i / segments;
			float x = radius * cosf(theta);
			float y = radius * sinf(theta);

			// 应用旋转变换
			float rotatedX = x * cos_theta - y * sin_theta;
			float rotatedY = x * sin_theta + y * cos_theta;

			// 转换到屏幕坐标
			vertices[i].position.x = centerPos.x + rotatedX;
			vertices[i].position.y = centerPos.y + rotatedY;

			// 圆形UV坐标映射（保持比例）
			vertices[i].tex_coord.x = (cosf(theta) + 1.0f) * 0.5f;
			vertices[i].tex_coord.y = (sinf(theta) + 1.0f) * 0.5f;

			// 颜色处理
			vertices[i].color = SDL_FColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		}

		// 颜色处理
		SDL_FColor finalColor = { 255, 255, 255, 255 };
		SDL_GetTextureColorModFloat(texture, &finalColor.r, &finalColor.g, &finalColor.b);
		SDL_GetTextureAlphaModFloat(texture, &finalColor.a);

		for (int i = 0; i <= segments; ++i) {
			vertices[i].color = finalColor;
		}

		// 索引数据（三角形扇）
		int indices[segments * 3] = { 0 };
		for (int i = 0; i < segments; ++i) {
			indices[i * 3 + 0] = 0;
			indices[i * 3 + 1] = i + 1;
			indices[i * 3 + 2] = (i + 2) % segments + 1;
		}

		// 执行渲染
		SDL_RenderGeometry(renderer, texture, vertices, segments + 1,
			indices, segments * 3);
	}
}