#pragma once
#include "render_shape.hpp"
namespace GearX {
	/*  *
		* @brief ʹ��SDL_RenderGeometryʵ��������ת��Ⱦ
		* @param renderer ��Ⱦ��ָ��
		* @param texture ����Ⱦ������
		* @param dst_rect Ŀ����Σ�λ�úͳߴ磩
		* @param angle ��ת�Ƕȣ��ȣ�˳ʱ��Ϊ����
		* @param center ��ת���ĵ㣨NULL��ʾĿ��������ģ�
		*/
	static SDL_Texture* Target_Texture = nullptr;
	void drawRect(SDL_Renderer* renderer, SDL_Texture* texture,
		const SDL_FRect* srcRect, const SDL_FRect* dst_rect,
		float angle, SDL_FPoint* center, TextureComponent::FlipMode flipmode) {
		// ��ȡ����ߴ�
		float texWidth, texHeight;
		SDL_GetTextureSize(texture, &texWidth, &texHeight);
		// ������ת����
		SDL_FPoint pivot;
		if (center) {
			pivot.x = dst_rect->x + center->x;
			pivot.y = dst_rect->y + center->y;
		}
		else {
			pivot.x = dst_rect->x + dst_rect->w / 2;
			pivot.y = dst_rect->y + dst_rect->h / 2;
		}

		// �����������꣨��һ����
		SDL_FRect src = srcRect ? *srcRect : SDL_FRect({ 0, 0, (float)texWidth, (float)texHeight });
		// ����תģʽ
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
		// �����һ��UV����
		SDL_FPoint uv[4] = {
			{ (float)flippedSrc.x / texWidth, (float)flippedSrc.y / texHeight },
			{ (float)(flippedSrc.x + flippedSrc.w) / texWidth, (float)flippedSrc.y / texHeight },
			{ (float)(flippedSrc.x + flippedSrc.w) / texWidth, (float)(flippedSrc.y + flippedSrc.h) / texHeight },
			{ (float)flippedSrc.x / texWidth, (float)(flippedSrc.y + flippedSrc.h) / texHeight }
		};
		// ����������㣨�ֲ����꣩
		const SDL_FPoint basePoints[] = {
			{0, 0},                   // ����
			{dst_rect->w, 0},         // ����
			{dst_rect->w, dst_rect->h},// ����
			{0, dst_rect->h}          // ����
		};

		// ��ת�任����
		const double radians = angle * M_PI / 180.0;
		const float cos_theta = (float)cos(radians);
		const float sin_theta = (float)sin(radians);

		// ������������
		SDL_Vertex vertices[4];
		for (int i = 0; i < 4; ++i) {
			// ת��Ϊ��ת�����������
			float dx = basePoints[i].x - (pivot.x - dst_rect->x);
			float dy = basePoints[i].y - (pivot.y - dst_rect->y);

			// Ӧ����ת����
			vertices[i].position.x = pivot.x + dx * cos_theta - dy * sin_theta;
			vertices[i].position.y = pivot.y + dx * sin_theta + dy * cos_theta;

			// ���������������ɫ
			vertices[i].tex_coord = uv[i];
			SDL_FColor color = SDL_FColor({ 1.0f,1.0f, 1.0f, 1.0f });
			SDL_GetTextureAlphaModFloat(texture, &color.a);
			SDL_GetTextureColorModFloat(texture, &color.r, &color.g, &color.b);
			vertices[i].color = color;
		}

		// ����������������������ɾ��Σ�
		const int indices[] = { 0, 1, 2, 0, 2, 3 };
	}
	void drawCircle(SDL_Renderer* renderer, SDL_Texture* texture,
		const SDL_FRect* srcRect, const SDL_FRect* dst_rect,
		float angle, SDL_FPoint* center, TextureComponent::FlipMode flipmode) {
		const float radius = dst_rect->w * 0.5f;  // Բ�뾶����������ȣ�
		const int segments = 100;                 // �ֶ���
		const float angleRad = angle * M_PI / 180.0f;
		const float angleStep = 2.0f * M_PI / segments;
		// ��ת�������
		const float cos_theta = cosf(angleRad);
		const float sin_theta = sinf(angleRad);

		// ����Բ��
		SDL_FPoint centerPos = {
			dst_rect->x + (center ? center->x : radius),
			dst_rect->y + (center ? center->y : radius)
		};

		// ��������
		SDL_Vertex vertices[segments + 1];

		for (int i = 0; i <= segments; ++i) {
			// ��׼Բ�β�������
			float theta = 2.0f * M_PI * i / segments;
			float x = radius * cosf(theta);
			float y = radius * sinf(theta);

			// Ӧ����ת�任
			float rotatedX = x * cos_theta - y * sin_theta;
			float rotatedY = x * sin_theta + y * cos_theta;

			// ת������Ļ����
			vertices[i].position.x = centerPos.x + rotatedX;
			vertices[i].position.y = centerPos.y + rotatedY;

			// Բ��UV����ӳ�䣨���ֱ�����
			vertices[i].tex_coord.x = (cosf(theta) + 1.0f) * 0.5f;
			vertices[i].tex_coord.y = (sinf(theta) + 1.0f) * 0.5f;

			// ��ɫ����
			vertices[i].color = SDL_FColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		}

		// ��ɫ����
		SDL_FColor finalColor = { 255, 255, 255, 255 };
		SDL_GetTextureColorModFloat(texture, &finalColor.r, &finalColor.g, &finalColor.b);
		SDL_GetTextureAlphaModFloat(texture, &finalColor.a);

		for (int i = 0; i <= segments; ++i) {
			vertices[i].color = finalColor;
		}

		// �������ݣ��������ȣ�
		int indices[segments * 3] = { 0 };
		for (int i = 0; i < segments; ++i) {
			indices[i * 3 + 0] = 0;
			indices[i * 3 + 1] = i + 1;
			indices[i * 3 + 2] = (i + 2) % segments + 1;
		}

		// ִ����Ⱦ
		SDL_RenderGeometry(renderer, texture, vertices, segments + 1,
			indices, segments * 3);
	}
}