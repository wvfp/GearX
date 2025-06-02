#include "starter.hpp"
#include "runtime/core/system/render/render_system.hpp"
#include "runtime/core/system/physics/physics_system.hpp"
#include "runtime/core/system/audio/audio_system.hpp"
#include "runtime/core/system/script/script_system.hpp"
#include "runtime/core/event/event.hpp"
GearX::Starter::~Starter(){
	RuntimeGlobalContext::audioSystem.Destory();
	RuntimeGlobalContext::shutdown();
}

void GearX::Starter::startGame(std::string worldUrl, Uint8 Framerate,std::array<int, 2> windowSize){
	namespace fs = std::filesystem;
	GearX::DefaultWindowSize = windowSize;
	RuntimeGlobalContext::init();
	RuntimeGlobalContext::current_path = fs::absolute(worldUrl).parent_path();
	RuntimeGlobalContext::world.loadWorld(worldUrl);
	RuntimeGlobalContext::isGameMode = true;
	framerate = Framerate;
	isGameRunnig = true;
	timeStep = 1.0f / framerate;
	GearX::Event::init();
	GearX::Event::registerCallback(SDL_EVENT_QUIT, [&](const SDL_Event&)->void {
			isGameRunnig = false;
		});
	RuntimeGlobalContext::audioSystem.Start();
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
	RuntimeGlobalContext::physicsSystem.tick(deltaTime);
	RuntimeGlobalContext::scriptSystem.tick(deltaTime);
	RuntimeGlobalContext::physicsSystem.updateTransform();

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
