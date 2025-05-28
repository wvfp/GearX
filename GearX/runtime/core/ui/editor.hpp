#pragma once
#include "ui.hpp"

namespace GearX {
	class ObjectEditor : public WindowUI {
	public:
		ObjectEditor() {};
		~ObjectEditor() {};
		void render() override;
	};
}