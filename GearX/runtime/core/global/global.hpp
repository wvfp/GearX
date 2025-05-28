#pragma once
#include "../../depencies.hpp"
#include "../../resources/asset_manager/asset_manager.hpp"
#include "../framework/component/component.hpp"
#include "../framework/world/world.hpp"
#include "../system/system.hpp"
#include "../utils/thread_pool.hpp"
#include "../ui/ui.hpp"

namespace GearX {
	class WindowUI;
	class World;
	class RenderSystem;
	class PhysicsSystem;
	class ScriptSystem;
	class AudioSystem;
	class AssetManager;
	class ThreadPool;
	extern std::array<int, 2> DefaultWindowSize;
	extern std::string DefaultWindowIcon;
	struct SDL_context {
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_Surface* surface = nullptr;
		SDL_Texture* texture = nullptr;
		SDL_Event event;
	};
	class RuntimeGlobalContext {
	public:
		static void init();
		static void shutdown();
		static SDL_context SDL_CONTEXT;
		static World world;
		static bool isInitialize;
		static RenderSystem renderSystem;
		static PhysicsSystem physicsSystem;
		static sol::state lua;
		static ScriptSystem scriptSystem;
		static std::shared_ptr<GearX::ThreadPool> threadPool;
		static std::filesystem::path current_path;
		static AssetManager assetManager;
		static AudioSystem audioSystem;
		static std::shared_ptr<WindowUI> UI;
		static bool isGameMode;
	};
}