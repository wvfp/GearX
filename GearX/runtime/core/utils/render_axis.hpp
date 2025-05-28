#include <SDL3/SDL.h>

namespace GearX{
	void drawArrow(SDL_Renderer* renderer, float x1, float y1, float x2, float y2,
		float arrowSize, float arrowWidth, SDL_FColor color);
	void drawObjectAxes(SDL_Renderer* renderer, SDL_FRect rect,float angle, SDL_FPoint center, float axisWidth, float arrowWidth);
}