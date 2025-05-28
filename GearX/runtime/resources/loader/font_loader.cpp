#include "font_loader.hpp"

TTF_Font* GearX::loadFont(const std::string& path, const Uint32 size) {
	TTF_Font* font = TTF_OpenFont(path.c_str(), size);
	if (font) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Font load failed£º%s", SDL_GetError());
		return nullptr;
	}
	else {
		return font;
	}
}