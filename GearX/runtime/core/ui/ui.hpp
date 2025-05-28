#pragma once
#include "../global/global.hpp"

namespace GearX {
	class WindowUI {
	public:
		virtual ~WindowUI() = default;
		virtual void render() = 0;
		virtual void update() {};
		virtual void cleanup() {};
		virtual void init() {};
	protected:
		// Init Flag
		bool isInit = false;
		//  Texture of UI
		SDL_Texture* UI_Texture = nullptr;
	};
}