#pragma once
#include "../system/script/script_system.hpp"
#include "../global/global.hpp"
// ����ļ�����GObject(��Ϸ����)���¼��Լ��������϶������� ...��
// ��ѡ�еĶ����ID Ĭ��Ϊ0����δѡ��
extern const Uint32 EVENT_GOBJECT_ONSELECTED;
extern const Uint32 EVENT_GOBJECT_ONUNSELECTED;
extern const Uint32 EVENT_GOBJECT_ONPRESSED;
extern const Uint32 EVENT_GOBJECT_ONRELEASED;
extern const Uint32 EVENT_GOBJECT_ONDRAG;
extern const Uint32 EVENT_GOBJECT_ONSCALE;
// ��Ϸģʽ�µ��¼�
// ������ڶ�����ʱ�������¼�
extern const Uint32 GEVENT_GOBJECT_MOUSEHOVER;
// �����������ʱ�������¼�
extern const Uint32 GEVENT_GOBJECT_MOUSEENTER;
// ������뿪����ʱ�������¼�
extern const Uint32 GEVENT_GOBJECT_MOUSELEAVE;
// ������ƶ�ʱ�������¼�
extern const Uint32 GEVENT_GOBJECT_MOUSEMOVE;
// �����������ʱ�������¼�
extern const Uint32 GEVENT_GOBJECT_MOUSECLICK;
// ����갴�¶���ʱ�������¼�
extern const Uint32 GEVENT_GOBJECT_MOUSEPRESS;
// ������ͷŶ���ʱ�������¼�
extern const Uint32 GEVENT_GOBJECT_MOUSERELEASE;
// ������������ʱ�������¼�
extern const Uint32 GEVENT_GOBJECT_MOUSEWHEEL;



namespace GearX {
	// �¼��ṹ�壬���ڴ洢�¼���Ϣ
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
					// �����¼�(SDL_Event)��lua
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