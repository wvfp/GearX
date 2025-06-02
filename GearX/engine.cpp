#include "engine.hpp"
#include "runtime/core/global/global.hpp"
#include "runtime/core/system/render/render_system.hpp"
#include "runtime/core/system/physics/physics_system.hpp"
#include "runtime/core/system/script/script_system.hpp"
#include "runtime/core/system/audio/audio_system.hpp"
#include <chrono>
#include <windows.h>
namespace GearX {
	class RuntimeGlobalContext;
	void GearXEngine::startEngine()
	{
		if (m_is_init == false) {
			initialize();
		}
		m_is_quit = false;
		m_fps_controller.setTargetFPS(RuntimeGlobalContext::DEFAULT_FPS);
	}

	void GearXEngine::shutdownEngine() {
		RuntimeGlobalContext::world.saveWorld(RuntimeGlobalContext::world.getWorldUrl());
		GearX::RuntimeGlobalContext::shutdown();
		m_is_quit = true;
		m_is_init = false;
	}

	void GearXEngine::initialize() {
		if (m_is_init == false) {
			GearX::RuntimeGlobalContext::init();
			m_fps_controller.reset();
			GearX::Event::init();
			GearX::Event::registerCallback(SDL_EVENT_QUIT, [&](const SDL_Event&)->void {
				RuntimeGlobalContext::isGameMode = false;
				m_is_quit = true;
				});
			m_is_init = true;
		}
	}

	void GearXEngine::clear() {
	}

	void GearXEngine::run() {
		while (m_is_quit == false) {
			this->tick();
			m_delta_time =  m_fps_controller.waitForNextFrame();
		}
	}

	void GearXEngine::tick() {
		if (RuntimeGlobalContext::isGameMode) {
			AudioSystem::Start();
		}
		else {
			AudioSystem::Destory();
		}
		GearX::Event::tick();
		logicalTick(m_delta_time);
		rendererTick(m_delta_time);
	}
	void GearXEngine::logicalTick(float delta_time) {
		static bool cached = false;
		static std::thread cached_thread = std::thread();
		static std::shared_ptr<Level> cached_level = std::make_shared<Level>();
		if (RuntimeGlobalContext::isGameMode) {
			if (cached == false) {
				cached = true;
				auto& world = RuntimeGlobalContext::world;
				std::thread([&]()->void {
					world.getCurrentLevel()->save();
					}).join();
				cached_thread = std::move(std::thread([&]()->void {
					static const std::regex jsonRegex(R"(\.(json)$)");
					if (std::regex_search(world.getCurrentLevelUrl(), jsonRegex)) {
						cached_level->load(world.getCurrentLevelUrl(), true);
					}
					else {
						cached_level->load(world.getCurrentLevelUrl(), false);
					}
					}));
				std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				std::tm* ptm = std::localtime(&now);
				char time_str[100];
				strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", ptm);
				//设置颜色
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				std::cout << "Time: ";
				SetConsoleTextAttribute(hConsole,FOREGROUND_GREEN);
				//输出
				std::cout << time_str;
				SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
				std::cout << " Game Mode : ";
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
				std::cout << "Start " << std::endl;
				//恢复颜色
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			}
			// delta_time 更新到lua
			RuntimeGlobalContext::lua["DeltaTime"] = delta_time;
			RuntimeGlobalContext::physicsSystem.tick(delta_time);
			RuntimeGlobalContext::scriptSystem.tick(delta_time);
		}
		else {
			RuntimeGlobalContext::scriptSystem.reset();
			RuntimeGlobalContext::physicsSystem.destroy();
			if (cached) {
				cached = false;
				// 等待缓存线程结束
				cached_thread.join();
				auto& level  = RuntimeGlobalContext::world.getCurrentLevel();
				RuntimeGlobalContext::world.setCurrentLevel(cached_level); 
				cached_level = level;
				std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				std::tm* ptm = std::localtime(&now);
				char time_str[100];
				strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", ptm);
				//设置颜色
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				std::cout << "Time: ";
				SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
				//输出
				std::cout <<time_str;
				SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
				std::cout << " Game Mode : ";
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
				std::cout << "End" << std::endl;
				//恢复颜色
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

			}
		}
		RuntimeGlobalContext::physicsSystem.updateTransform();
	}

	float gridSize = 20;
	std::array<float, 4> gridColor = { 0.3f,0.3f,0.3f,0.7f };
	static void drawGrid() {
		int currentWidth = 0, currentHeight = 0;
		std::array<float, 4> v_Color = { 0,0,0,0 };
		float* Color = v_Color.data();
		auto rdr = SDL_GetRendererFromTexture(RuntimeGlobalContext::SDL_CONTEXT.texture);
		SDL_GetRenderDrawColorFloat(rdr,
			Color, Color + 1, Color + 2, Color + 3);
		SDL_GetWindowSize(RuntimeGlobalContext::SDL_CONTEXT.window,
			&currentWidth, &currentHeight);
		SDL_SetRenderDrawColorFloat(rdr,
			gridColor[0], gridColor[1], gridColor[2], gridColor[3]);
		SDL_SetRenderScale(rdr, 8.0f, 8.0f);
		for (float x = 0; x <= currentWidth; x += gridSize) {
			SDL_RenderLine(rdr,
				x, 0, x, currentHeight);
		}
		for (float y = 0; y <= currentHeight; y += gridSize) {
			SDL_RenderLine(rdr,
				0, y, currentWidth, y);
		}
		SDL_SetRenderDrawColorFloat(rdr,
			v_Color[0], v_Color[1], v_Color[2], v_Color[3]);
		SDL_SetRenderScale(rdr, 1, 1);
	}
	static void drawGridControler() {
		int currentWidth = 0, currentHeight = 0;
		SDL_GetWindowSize(RuntimeGlobalContext::SDL_CONTEXT.window,
			&currentWidth, &currentHeight);
		float length = sqrt(currentWidth * currentWidth + currentHeight * currentHeight);
		ImGui::Begin(u8"网格控制");
		ImGui::SliderFloat(u8"间距", &gridSize, length / 50, length / 20);
		ImGui::ColorEdit4(u8"网格颜色", gridColor.data());
		ImGui::End();
	}
	void GearXEngine::rendererTick(float delta_time) {
		// 设置并清理渲染纹理
		{
			SDL_SetRenderTarget(RuntimeGlobalContext::SDL_CONTEXT.renderer, RuntimeGlobalContext::SDL_CONTEXT.texture);
			SDL_SetRenderDrawColor(RuntimeGlobalContext::SDL_CONTEXT.renderer, 40, 40, 40, 255);
			SDL_RenderClear(RuntimeGlobalContext::SDL_CONTEXT.renderer);
			if (!RuntimeGlobalContext::isGameMode)
				drawGrid();
		}
		// World 渲染 和 逻辑处理
		RuntimeGlobalContext::renderSystem.tick(delta_time);
		//  渲染到屏幕纹理上
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
		// UI 渲染，ImGui
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		RuntimeGlobalContext::UI->render();
		drawGridControler();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), RuntimeGlobalContext::SDL_CONTEXT.renderer);
		// 渲染到屏幕上
		SDL_RenderPresent(GearX::RuntimeGlobalContext::SDL_CONTEXT.renderer);
	}
} // namespace GearX