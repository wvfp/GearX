#include "GearX/engine.hpp"
using namespace GearX;

// ÔÚmain.cppÖĞ£º
int main(int argc, char* argv[]) {
	GearX::GearXEngine engine;
	engine.startEngine();
	engine.run();
	engine.shutdownEngine();
	return 0;
}