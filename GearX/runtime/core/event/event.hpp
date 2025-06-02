#pragma once
#include "../system/script/script_system.hpp"
#include "../global/global.hpp"
// 这个文件放置GObject(游戏对象)的事件以及操作（拖动，缩放 ...）
// 被选中的对象的ID 默认为0，即未选中
extern const Uint32 EVENT_GOBJECT_ONSELECTED;
extern const Uint32 EVENT_GOBJECT_ONUNSELECTED;
extern const Uint32 EVENT_GOBJECT_ONPRESSED;
extern const Uint32 EVENT_GOBJECT_ONRELEASED;
extern const Uint32 EVENT_GOBJECT_ONDRAG;
extern const Uint32 EVENT_GOBJECT_ONSCALE;
// 游戏模式下的事件
// 当鼠标在对象上时触发的事件
extern const Uint32 GEVENT_GOBJECT_MOUSEHOVER;
// 当鼠标进入对象时触发的事件
extern const Uint32 GEVENT_GOBJECT_MOUSEENTER;
// 当鼠标离开对象时触发的事件
extern const Uint32 GEVENT_GOBJECT_MOUSELEAVE;
// 当鼠标移动时触发的事件
extern const Uint32 GEVENT_GOBJECT_MOUSEMOVE;
// 当鼠标点击对象时触发的事件
extern const Uint32 GEVENT_GOBJECT_MOUSECLICK;
// 当鼠标按下对象时触发的事件
extern const Uint32 GEVENT_GOBJECT_MOUSEPRESS;
// 当鼠标释放对象时触发的事件
extern const Uint32 GEVENT_GOBJECT_MOUSERELEASE;
// 当鼠标滚动对象时触发的事件
extern const Uint32 GEVENT_GOBJECT_MOUSEWHEEL;



namespace GearX {
	// 事件结构体，用于存储事件信息
	typedef struct {
		Uint32 type;
		Uint32 id;
		Uint32 timestamp;
		float x, y;
		float xrel, yrel;
		Uint8 button;
		Uint8 clicks;
		int direction;
	}ObjectEvent;
	class ScriptSystem;
	// Filter for ImGui
	extern bool eventFilter(SDL_Event* event);
	class Event {
	public:
		using CallBackType = std::function<void(const SDL_Event&)>;
		using CallBackListType = std::vector<CallBackType>;
		using CallBackMapType = std::map<Uint32, CallBackListType>;
		static  CallBackMapType callbackMap;
		static std::vector<std::pair<sol::table,sol::function>> LuaCallBackMap;
	public:
		static void tick(void) {
			SDL_Event& event = RuntimeGlobalContext::SDL_CONTEXT.event;

			while (SDL_PollEvent(&event)) {
				ImGui_ImplSDL3_ProcessEvent(&event);
				if (eventFilter(&event)) {
					if (callbackMap.count(event.type) > 0) {
						for (auto& callback : callbackMap[event.type])
							callback(event);
					}
					// 跟新事件(SDL_Event)到lua
					if (RuntimeGlobalContext::isGameMode) {
						auto& tb = GearX::Event::registerEventToLua(RuntimeGlobalContext::scriptSystem.getLuaEnv());
						for (auto& callback : LuaCallBackMap)
							callback.second(callback.first,tb);
					}
				}
			}
		}
		static sol::table registerEventToLua(sol::environment& env , bool = false);
		static void registerCallback(Uint32 et, CallBackType cbt) {
			callbackMap[et].push_back(cbt);
		}
		static void init(void);
	};
}