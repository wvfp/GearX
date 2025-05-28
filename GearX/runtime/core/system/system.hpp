#pragma once
#include "../../depencies.hpp"
namespace GearX {
	class System{
	public:
		virtual void tick(float deltaTime) = 0;
	};

}
