#pragma once
#include "../system.hpp"

namespace GearX {
	class RenderSystem :public System {
	public:
		void tick(float deltaTime)override;
	};
}