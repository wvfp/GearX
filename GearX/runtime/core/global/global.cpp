#include "global.hpp"
#include "../ui/editor.hpp"
#include "../ui/imgui_style.hpp"
#include "../system/render/render_system.hpp"
#include "../system/physics/physics_system.hpp"
#include "../system/script/script_system.hpp"
#include "../system/audio/audio_system.hpp"
namespace GearX {
	Uint8 RuntimeGlobalContext::DEFAULT_FPS = 60.0f;
	std::array<int, 2> DefaultWindowSize = { 2400, 1280 };
	std::string DefaultWindowTitle = "GearXEngine";
	std::string DefaultWindiwIcon = "./asset/default/icon_round.png";
	SDL_Surface* WindowIcon = nullptr;
	SDL_context RuntimeGlobalContext::SDL_CONTEXT = {};
	AssetManager RuntimeGlobalContext::assetManager = AssetManager();
	std::shared_ptr<WindowUI> RuntimeGlobalContext::UI = std::make_shared<ObjectEditor>();
	RenderSystem  RuntimeGlobalContext::renderSystem = RenderSystem();
	PhysicsSystem  RuntimeGlobalContext::physicsSystem = PhysicsSystem();
	sol::state RuntimeGlobalContext::lua = sol::state();
	AudioSystem RuntimeGlobalContext::audioSystem = AudioSystem();
	ScriptSystem RuntimeGlobalContext::scriptSystem = ScriptSystem();
	std::filesystem::path  RuntimeGlobalContext::current_path = std::filesystem::absolute(std::filesystem::path("./"));
	//默认使用当前硬件支持线程数
	std::shared_ptr<ThreadPool> RuntimeGlobalContext::threadPool = std::make_shared<ThreadPool>();
	World RuntimeGlobalContext::world = World();
	bool RuntimeGlobalContext::isGameMode = false;
	bool RuntimeGlobalContext::isInitialize{ false };
	void RuntimeGlobalContext::init() {
		// Initialize the global context
		if (!isInitialize) {
			// Initialize SDL All
			SDL_InitFlags initFlags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS|
			 SDL_INIT_HAPTIC | SDL_INIT_SENSOR ;
			if (SDL_Init(initFlags) == false) {
				SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to initialize SDL: %s", SDL_GetError());
				exit(1);
			}
			//  Set OpenGL Attributes
			SDL_CONTEXT.window = SDL_CreateWindow(DefaultWindowTitle.c_str(), DefaultWindowSize[0], DefaultWindowSize[1], SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
			if (SDL_CONTEXT.window == nullptr) {
				SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to create window: %s", SDL_GetError());
				exit(1);
			}
			WindowIcon = IMG_Load(DefaultWindiwIcon.c_str());
			SDL_SetWindowIcon(SDL_CONTEXT.window, WindowIcon);
			SDL_CONTEXT.renderer = SDL_CreateRenderer(SDL_CONTEXT.window, nullptr);
			if (SDL_CONTEXT.renderer == nullptr) {
				SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to create renderer: %s", SDL_GetError());
				exit(1);
			}
			SDL_SetRenderVSync(SDL_CONTEXT.renderer, 1);
			SDL_CONTEXT.surface = SDL_GetWindowSurface(SDL_CONTEXT.window);
			if (SDL_CONTEXT.surface == nullptr) {
				SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to get surface: %s", SDL_GetError());
				exit(1);
			}
			int width, height;
			SDL_GetWindowSize(SDL_CONTEXT.window, &width, &height);
			SDL_CONTEXT.texture = SDL_CreateTexture(SDL_CONTEXT.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
			if (SDL_CONTEXT.texture == nullptr) {
				SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to create texture: %s", SDL_GetError());
				exit(1);
			}
			SDL_SetTextureScaleMode(SDL_CONTEXT.texture, SDL_ScaleMode::SDL_SCALEMODE_LINEAR);
			//Initialize Dear ImGUI
			ImGui::CreateContext();
			ImGui_ImplSDL3_InitForSDLRenderer(SDL_CONTEXT.window, SDL_CONTEXT.renderer);
			ImGui_ImplSDLRenderer3_Init(SDL_CONTEXT.renderer);
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			//ImGui::DockSpaceOverViewport();
			SetImGuiWhiteStyle();
			//Initialize Window Background Color
			SDL_SetRenderDrawColor(SDL_CONTEXT.renderer, 0x55, 0x55, 0x55, 0x55);
			SDL_Texture* WindowIconTexture = SDL_CreateTextureFromSurface(SDL_CONTEXT.renderer, WindowIcon);
			SDL_RenderClear(SDL_CONTEXT.renderer);
			SDL_FRect rect = { (float)DefaultWindowSize[0]/4, (float)DefaultWindowSize[1]/4, 
				(float)DefaultWindowSize[0] / 2, (float)DefaultWindowSize[1] / 2};
			SDL_RenderTexture(SDL_CONTEXT.renderer, WindowIconTexture, nullptr, &rect);
			SDL_DestroyTexture(WindowIconTexture);
			SDL_RenderPresent(SDL_CONTEXT.renderer);
			isInitialize = true;
		}
	}
	void RuntimeGlobalContext::shutdown() {
		// Shutdown the global context
		ImGui_ImplSDLRenderer3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
		SDL_DestroyRenderer(SDL_CONTEXT.renderer);
		SDL_DestroySurface(SDL_CONTEXT.surface);
		SDL_DestroySurface(WindowIcon);
		SDL_DestroyTexture(SDL_CONTEXT.texture);
		SDL_DestroyWindow(SDL_CONTEXT.window);
		SDL_memset(&SDL_CONTEXT, 0, sizeof(SDL_CONTEXT));
		SDL_Quit();
		isInitialize = false;
	}
}