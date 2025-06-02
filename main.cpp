#include "GearX/engine.hpp"
using namespace GearX;

// ��main.cpp�У�
int main(int argc, char* argv[]) {
	GearX::GearXEngine engine;
	engine.startEngine();
	try {
		engine.run();
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	engine.shutdownEngine();
	return 0;
}