#include "texture_loader.hpp"

SDL_Texture* GearX::loadTexture(const std::string& path) {
	if (path.empty() || path == "") {
		return nullptr;
	}
	SDL_Texture* texture = IMG_LoadTexture(RuntimeGlobalContext::SDL_CONTEXT.renderer, path.c_str());
	if (texture) {
		return texture;
	}
	else {
		SDL_Log("Failed to load texture from path: %s", path.c_str());
		return nullptr;
	}
}