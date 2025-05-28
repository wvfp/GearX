#pragma once
#include "../global/global.hpp"
namespace GearX {
	// Filter for ImGui
	extern bool eventFilter(SDL_Event* event);
	class Event {
	public:
		using CallBackType = std::function<void(const SDL_Event&)>;
		using CallBackListType = std::vector<CallBackType>;
		using CallBackMapType = std::map<SDL_EventType, CallBackListType>;
		static  CallBackMapType callbackMap;
	public:
		static void tick(void) {
			SDL_Event& event = RuntimeGlobalContext::SDL_CONTEXT.event;
			SDL_PumpEvents();
			while (SDL_PollEvent(&event)) {
				ImGui_ImplSDL3_ProcessEvent(&event);
				if (eventFilter(&event)) {
					if (callbackMap.count(static_cast<SDL_EventType>(event.type)) > 0) {
						for (auto& callback : callbackMap[static_cast<SDL_EventType>(event.type)])
							callback(event);
					}
				}
			}
		}
		static void registerEventToLua(sol::state& lua_state);
		static void registerCallback(SDL_EventType et, CallBackType cbt) {
			callbackMap[et].push_back(cbt);
		}
		static void init(void);
	};
}