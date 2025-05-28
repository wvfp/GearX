#include "render_axis.hpp"
#include <cmath>
#define M_PI 3.1415926535
namespace GearX {
	void drawArrow(SDL_Renderer* renderer, float x1, float y1, float x2, float y2,
		float arrowSize, float arrowWidth, SDL_FColor color) {
		double dx = x2 - x1;
		double dy = y2 - y1;
		double length = std::sqrt(dx * dx + dy * dy);
		double unitX = dx / length;
		double unitY = dy / length;

		float scaleFactor = std::fmin(length / 100.0f, 2.0f);
		arrowSize *= scaleFactor;
		arrowWidth *= scaleFactor;

		SDL_Vertex vertices[3];
		const float angle = std::atan2(dy, dx);
		const float branchLen = arrowSize + arrowWidth * 0.5f;

		vertices[0].position = { static_cast<float>(x2), static_cast<float>(y2) };
		vertices[0].color = color;

		vertices[1].position = {
			static_cast<float>(x2 - branchLen * std::cos(angle - M_PI / 6)),
			static_cast<float>(y2 - branchLen * std::sin(angle - M_PI / 6))
		};
		vertices[2].position = {
			static_cast<float>(x2 - branchLen * std::cos(angle + M_PI / 6)),
			static_cast<float>(y2 - branchLen * std::sin(angle + M_PI / 6))
		};
		vertices[1].color = vertices[2].color = color;

		SDL_RenderGeometry(renderer, nullptr, vertices, 3, nullptr, 0);

		float lineLength = length - arrowSize;
		float halfWidth = arrowWidth / 2.0f;

		SDL_Vertex rectVertices[4];
		rectVertices[0].position = { static_cast<float>(x1 - halfWidth * unitY), static_cast<float>(y1 + halfWidth * unitX) };
		rectVertices[1].position = { static_cast<float>(x1 + halfWidth * unitY), static_cast<float>(y1 - halfWidth * unitX) };
		rectVertices[2].position = { static_cast<float>(x1 + lineLength * unitX + halfWidth * unitY), static_cast<float>(y1 + lineLength * unitY - halfWidth * unitX) };
		rectVertices[3].position = { static_cast<float>(x1 + lineLength * unitX - halfWidth * unitY), static_cast<float>(y1 + lineLength * unitY + halfWidth * unitX) };

		for (int i = 0; i < 4; ++i) {
			rectVertices[i].color = color;
			rectVertices[i].tex_coord = { 0.0f, 0.0f };
		}

		const int indices[6] = { 0, 1, 2, 2, 3, 0 };

		SDL_RenderGeometry(renderer, nullptr, rectVertices, 4, indices, 6);
	}

	SDL_FPoint GetRotatedPoint(SDL_FPoint point, SDL_FPoint center, float angle) {
		SDL_FPoint result = { 0.0f, 0.0f };
		float rad = angle * M_PI / 180.0f;
		float cosAngle = std::cos(rad);
		float sinAngle = std::sin(rad);

		float dx = point.x - center.x;
		float dy = point.y - center.y;

		result.x = dx * cosAngle - dy * sinAngle;
		result.y = dx * sinAngle + dy * cosAngle;

		result.x += center.x;
		result.y += center.y;

		return result;
	}

	void drawObjectAxes(SDL_Renderer* renderer, SDL_FRect rect, float angle, SDL_FPoint center, float axisWidth, float arrowWidth) {
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_FColor xAxisColor = { 1, 0, 0, 0.65 };
		SDL_FColor yAxisColor = { 0, 1, 0, 0.65 };
		center.x += rect.x;
		center.y += rect.y;
		SDL_FPoint endX = center;
		SDL_FPoint endY = center;
		float length = std::sqrt(rect.w * rect.w + rect.h * rect.h) / 2.5f;
		endX.x += length;
		endY.y += length;
		endX = GetRotatedPoint(endX, center, angle);
		endY = GetRotatedPoint(endY, center, angle);

		drawArrow(renderer, center.x, center.y, endX.x, endX.y, axisWidth, arrowWidth, xAxisColor);
		drawArrow(renderer, center.x, center.y, endY.x, endY.y, axisWidth, arrowWidth, yAxisColor);
		SDL_SetRenderDrawColorFloat(renderer, 0.6f, 0.6f, 0.6f, 0.99f);
		float scaleFactor = std::fmin(length / 100.0f, 2.0f);
		arrowWidth *= scaleFactor;
		int radius = arrowWidth * 0.5f;
		for (int w = 0; w < radius * 2; w++) {
			for (int h = 0; h < radius * 2; h++) {
				int dx = radius - w;
				int dy = radius - h;
				if ((dx * dx + dy * dy) <= (radius * radius)) {
					SDL_RenderPoint(renderer, static_cast<int>(center.x + dx), static_cast<int>(center.y + dy));
				}
			}
		}
	}
}