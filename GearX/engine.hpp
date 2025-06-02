#pragma once

#include <atomic>
#include <chrono>
#include <string>
#include "runtime/core/global/global.hpp"
#include "runtime/core/event/event.hpp"
#include "runtime/resources/loader/texture_loader.hpp"
#include "runtime/core/utils/fps_controler.hpp"
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
	protected:
		void logicalTick(float delta_time);
		void rendererTick(float delta_time);
	private:
		bool m_is_quit{ false };
		bool m_is_init{ false };
		FrameRateController m_fps_controller;
		float m_delta_time{ 0.0f };
	};
} // namespace GearX