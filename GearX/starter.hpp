#pragma once
#include "runtime/core/global/global.hpp"

namespace GearX {
    class Starter {
    public:
        Starter() {};
        ~Starter();
        void startGame(std::string woldUrl="world",Uint8 Framerate = 60,std::array<int,2> windowSize = {2400,1400});
        void tick();
        void logicTick(float deltaTime);
        void renderTick(float deltaTime);
        bool isRunning() const { return isGameRunnig; }
    private:
        float deltaTime{0.0f};
        float playingTime{0.0f};
        bool isGameRunnig{true};
        Uint8 framerate{60};
        float timeStep{1.0f/60.0f};
    };
}
