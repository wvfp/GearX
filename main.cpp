#include "GearX/engine.hpp"
using namespace GearX;

// ��main.cpp�У�
int main(int argc, char* argv[]) {
	GearX::GearXEngine engine;
	engine.startEngine();
	engine.run();
	engine.shutdownEngine();
	return 0;
}