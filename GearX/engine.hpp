#pragma once

#include <atomic>
#include <chrono>
#include <string>
#include "runtime/core/global/global.hpp"
#include "runtime/core/event/event.hpp"
#include "SDL3_gfx/SDL3_framerate.h"
#include "runtime/resources/loader/texture_loader.hpp"

namespace GearX
{
	class GearXEngine
	{
	public:
		void startEngine();
		void shutdownEngine();
		void initialize();
		void clear();

		bool isQuit() const { return m_is_quit; }
		void run();
		void tick();

		int getFPS() const { return m_manager.rate; }
		void setFPS(Uint32 fps) {
			SDL_setFramerate(&m_manager, fps);
			m_delta_time = 1.0f / static_cast<float>(fps);
		}
	protected:
		void logicalTick(float delta_time);
		void rendererTick(float delta_time);
	private:
		bool m_is_quit{ false };
		bool m_is_init{ false };
		FPSmanager m_manager;
		float m_delta_time{ 0.0f };
	};
} // namespace GearX