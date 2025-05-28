#include "starter.hpp"
#include "runtime/core/system/render/render_system.hpp"
#include "runtime/core/system/physics/physics_system.hpp"
#include "runtime/core/system/script/script_system.hpp"
#include "runtime/core/event/event.hpp"
GearX::Starter::~Starter(){
	RuntimeGlobalContext::shutdown();
}

void GearX::Starter::startGame(std::string woldUrl, Uint8 Framerate,std::array<int, 2> windowSize){
	GearX::DefaultWindowSize = windowSize;
	RuntimeGlobalContext::init();
	RuntimeGlobalContext::world.loadWorld(woldUrl);
	framerate = Framerate;
	isGameRunnig = true;
	timeStep = 1.0f / framerate;
	GearX::Event::init();
	GearX::Event::registerCallback(SDL_EVENT_QUIT, [&](const SDL_Event&)->void {
			isGameRunnig = false;
		});
}

void GearX::Starter::tick(){
	static float lastTime = SDL_GetTicks() / 1000.0f;
	if(isGameRunnig) {
		logicTick(deltaTime);
		renderTick(deltaTime);
		// ����֡��
		{
			float currentTime = SDL_GetTicks() / 1000.0f;
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;
			if (deltaTime < timeStep) {
				SDL_Delay(static_cast<Uint32>(timeStep * 1000.0f - deltaTime * 1000.0f));
			}
		}
	}
}

void GearX::Starter::logicTick(float deltaTime){
	GearX::Event::tick();
	// delta_time ���µ�lua
	RuntimeGlobalContext::lua["DeltaTime"] = deltaTime;
	// �����¼�(SDL_Event)��lua
	GearX::Event::registerEventToLua(RuntimeGlobalContext::lua);
	RuntimeGlobalContext::physicsSystem.tick(deltaTime);
	RuntimeGlobalContext::physicsSystem.updateTransform();
	RuntimeGlobalContext::scriptSystem.tick(deltaTime);

}

void GearX::Starter::renderTick(float deltaTime){
	// ���ò�������Ⱦ����
	{
		SDL_SetRenderTarget(RuntimeGlobalContext::SDL_CONTEXT.renderer, RuntimeGlobalContext::SDL_CONTEXT.texture);
		SDL_SetRenderDrawColor(RuntimeGlobalContext::SDL_CONTEXT.renderer, 40, 40, 40, 255);
		SDL_RenderClear(RuntimeGlobalContext::SDL_CONTEXT.renderer);
	}
	// World ��Ⱦ �� �߼�����
	RuntimeGlobalContext::renderSystem.tick(deltaTime);
	//  ��Ⱦ����Ļ������
	{
		SDL_SetRenderTarget(RuntimeGlobalContext::SDL_CONTEXT.renderer, nullptr);
		SDL_SetRenderScale(RuntimeGlobalContext::SDL_CONTEXT.renderer, 1, 1);
		SDL_RenderClear(RuntimeGlobalContext::SDL_CONTEXT.renderer);
		if (RuntimeGlobalContext::world.getCurrentLevel()) {
			auto rect = RuntimeGlobalContext::world.getCurrentLevel()->getRenderRect();
			SDL_FRect s_rect = {
				rect[0],rect[1],
				rect[2] * 8,rect[3] * 8 };
			SDL_RenderTexture(RuntimeGlobalContext::SDL_CONTEXT.renderer, GearX::RuntimeGlobalContext::SDL_CONTEXT.texture, &s_rect, NULL);
		}
		else
			SDL_RenderTexture(RuntimeGlobalContext::SDL_CONTEXT.renderer, GearX::RuntimeGlobalContext::SDL_CONTEXT.texture, NULL, NULL);
	}
	// ��Ⱦ����Ļ��
	SDL_RenderPresent(GearX::RuntimeGlobalContext::SDL_CONTEXT.renderer);

}
