#pragma once
#include "../system.hpp"
namespace GearX {
	static bool isPhysicsInit = false;
	class  PhysicsSystem :public System {
	public:
		void tick(float deltaTime)override;
		void updateTransform();
		void destroy();
	};
}