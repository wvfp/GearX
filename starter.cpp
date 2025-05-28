#include "GearX/starter.hpp"
using namespace GearX;
int main(int argc, char* argv[]) {
    Starter starter;
    starter.startGame("example/world.json");
    while (starter.isRunning()) {
        starter.tick();
    }
    return 0;
}
