#include "object_event.hpp"
#include "event.hpp"
// 初始化静态成员变量
GearX::Event::CallBackMapType GearX::Event::callbackMap = GearX::Event::CallBackMapType();
// 处理窗口大小变化
static void processWindowResized(const SDL_Event& event) {
	SDL_Renderer* renderer = GearX::RuntimeGlobalContext::SDL_CONTEXT.renderer;
	SDL_Window* window = GearX::RuntimeGlobalContext::SDL_CONTEXT.window;
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	SDL_Rect rect = { 0,0,w,h };
	SDL_SetRenderViewport(renderer, &rect);
}
bool isOnPressed = false;
static void processWhellChange(const SDL_Event& event) {
	if (GearX::RuntimeGlobalContext::isGameMode) {
		return;
	}
	using namespace GearX;
	if (SelectedObj == 0) {
		auto& level = RuntimeGlobalContext::world.getCurrentLevel();
		if (level) {
			int w = 0, h = 0;
			SDL_GetWindowSize(RuntimeGlobalContext::SDL_CONTEXT.window, &w, &h);
			//坐标映射
			auto rect = level->getRenderRect();
			float c_x = event.wheel.mouse_x * rect[2] / (float)w;
			float c_y = event.wheel.mouse_y * rect[3] / (float)h;
			c_x += rect[0], c_y += rect[1];
			float ds{ event.wheel.y * 5.0f };
			if (c_x > rect[0]) {
				rect[0] += ds;
			}
			if (rect[0] < 0) {
				rect[0] = 0;
			}
			if (c_y > rect[1]) {
				rect[1] += ds;
			}
			if (rect[1] < 0) {
				rect[1] = 0;
			}
			rect[2] -= ds;
			rect[3] -= ds;
			if (rect[2] > level->getTargetTextureSize()[0])
				rect[2] = level->getTargetTextureSize()[0];
			if (rect[3] > level->getTargetTextureSize()[1])
				rect[3] = level->getTargetTextureSize()[1];
			level->setRenderRect(rect);
		}
	}
}
static void processMouseMotion(const SDL_Event& event) {
	if (GearX::RuntimeGlobalContext::isGameMode) {
		return;
	}
	using namespace GearX;
	if (SelectedObj == 0 && isOnPressed) {
		auto& level = RuntimeGlobalContext::world.getCurrentLevel();
		if (level) {
			int w = 0, h = 0;
			SDL_GetWindowSize(RuntimeGlobalContext::SDL_CONTEXT.window, &w, &h);
			//坐标映射
			auto rect = level->getRenderRect();
			float c_x = event.motion.xrel * rect[2] / (float)w;
			float c_y = event.motion.yrel * rect[3] / (float)h;
			rect[0] -= c_x;
			rect[1] -= c_y;
			if (rect[2] + rect[0] > level->getTargetTextureSize()[0])
				rect[0] = level->getTargetTextureSize()[0] - rect[2];
			if (rect[3] + rect[1] > level->getTargetTextureSize()[1])
				rect[1] = level->getTargetTextureSize()[1] - rect[3];
			if (rect[0] < 0) {
				rect[0] = 0;
			}
			if (rect[1] < 0) {
				rect[1] = 0;
			}
			level->setRenderRect(rect);
		}
	}
}
static void onKeyMapChanged(const SDL_Event& event) {
	int numkeys = 0;
	GearX::RuntimeGlobalContext::lua["KeyState"] = SDL_GetKeyboardState(&numkeys);
	GearX::RuntimeGlobalContext::lua["KeyStateSize"] = numkeys;
}
void GearX::Event::registerEventToLua(sol::state& lua_state){
	static bool is_registered = false;
	static auto table = lua_state.create_table();
	static auto table_mouse = lua_state.create_table();
	static auto table_keyboard = lua_state.create_table();
	if (!is_registered) {
		// 注册事件枚举类型，mouse, keyboard
		auto event_type = lua_state.create_table();
		auto mouse_type = lua_state.create_table();
		auto keyboard_type = lua_state.create_table();
		mouse_type["ButtonDown"] = SDL_EVENT_MOUSE_BUTTON_DOWN;
		mouse_type["ButtonUp"] = SDL_EVENT_MOUSE_BUTTON_UP;
		mouse_type["Motion"] = SDL_EVENT_MOUSE_MOTION;
		mouse_type["Wheel"] = SDL_EVENT_MOUSE_WHEEL;
		keyboard_type["KeyMapChanged"] = SDL_EVENT_KEYMAP_CHANGED;
		keyboard_type["KeyDown"] = SDL_EVENT_KEY_DOWN;
		keyboard_type["KeyUp"] = SDL_EVENT_KEY_UP;
		event_type["Mouse"] = mouse_type;
		event_type["Keyboard"] = keyboard_type;
		lua_state["EventType"] = event_type;
		// 各个表中的变量初始化为nil
		table["type"] = sol::nil;
		table_mouse["button"] = sol::nil;
		table_mouse["x"] = sol::nil;
		table_mouse["y"] = sol::nil;
		table_mouse["clicks"] = sol::nil;
		table_mouse["padding"] = sol::nil;
		table_mouse["reserved"] = sol::nil;
		table_mouse["timestamp"] = sol::nil;
		table_mouse["xrel"] = sol::nil;
		table_mouse["yrel"] = sol::nil;
		table_mouse["state"] = sol::nil;
		table_mouse["direction"] = sol::nil;
		table_mouse["timestamp"] = sol::nil;
		table_mouse["reserved"] = sol::nil;
		table_keyboard["keysym"] = sol::nil;
		table_keyboard["scancode"] = sol::nil;
		table_keyboard["repeat"] = sol::nil;
		table_keyboard["reserved"] = sol::nil;
		table_keyboard["timestamp"] = sol::nil;
		table["mouse"] = table_mouse;
		table["keyboard"] = table_keyboard;
		is_registered = true;
	}
	table["type"] = RuntimeGlobalContext::SDL_CONTEXT.event.type;
	auto& event = RuntimeGlobalContext::SDL_CONTEXT.event;
	switch (event.type){
	case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_DOWN:
		table_mouse["button"] = event.button.button;
		table_mouse["x"] = event.button.x;
		table_mouse["y"] = event.button.y;
		table_mouse["clicks"] = event.button.clicks;
		table_mouse["padding"] = event.button.padding;
		table_mouse["reserved"] = event.button.reserved;
		table_mouse["timestamp"] = event.button.timestamp;
		table["mouse"] = table_mouse;
		break;
	case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_UP:
		table_mouse["button"] = event.button.button;
		table_mouse["x"] = event.button.x;
		table_mouse["y"] = event.button.y;
		table_mouse["clicks"] = event.button.clicks;
		table_mouse["padding"] = event.button.padding;
		table_mouse["reserved"] = event.button.reserved;
		table_mouse["timestamp"] = event.button.timestamp;
		table["mouse"] = table_mouse;
		break;
	case SDL_EventType::SDL_EVENT_MOUSE_MOTION:
		table_mouse["x"] = event.motion.x;
		table_mouse["y"] = event.motion.y;
		table_mouse["xrel"] = event.motion.xrel;
		table_mouse["yrel"] = event.motion.yrel;
		table_mouse["state"] = event.motion.state;
		table_mouse["timestamp"] = event.motion.timestamp;
		table["mouse"] = table_mouse;
		break;
	case SDL_EventType::SDL_EVENT_MOUSE_WHEEL:
		table_mouse["x"] = event.wheel.mouse_x;
		table_mouse["y"] = event.wheel.mouse_y;
		table_mouse["xrel"] = event.wheel.x;
		table_mouse["yrel"] = event.wheel.y;
		table_mouse["direction"] = event.wheel.direction;
		table_mouse["timestamp"] = event.wheel.timestamp;
		table_mouse["reserved"] =  event.wheel.reserved;
		table["mouse"] = table_mouse;
		break;
	case SDL_EventType::SDL_EVENT_KEYMAP_CHANGED:
	case SDL_EventType::SDL_EVENT_KEY_DOWN:
	case SDL_EventType::SDL_EVENT_KEY_UP:
		table_keyboard["keysym"] = static_cast<Uint32>(event.key.key);
		table_keyboard["scancode"] = static_cast<Uint32>(event.key.scancode);
		table_keyboard["repeat"] = event.key.repeat;
		table_keyboard["reserved"] = event.key.reserved;
		table_keyboard["timestamp"] = event.key.timestamp;
		table["keyboard"] = table_keyboard;
		break;
	default:
		break;
	}
	lua_state["Event"] = table;
}
void GearX::Event::init(void) {
	registerCallback(SDL_EVENT_WINDOW_RESIZED, processWindowResized);
	registerCallback(SDL_EVENT_MOUSE_WHEEL, processWhellChange);
	registerCallback(SDL_EVENT_MOUSE_MOTION, processMouseMotion);
	registerCallback(SDL_EVENT_MOUSE_BUTTON_DOWN, GObject_Select);
	registerCallback(SDL_EVENT_MOUSE_WHEEL, GObject_Scale);
	registerCallback(SDL_EVENT_MOUSE_BUTTON_DOWN, GObject_Press);
	registerCallback(SDL_EVENT_MOUSE_BUTTON_UP, GObject_Release);
	registerCallback(SDL_EVENT_MOUSE_MOTION, GObject_Drag);
	registerCallback(SDL_EVENT_MOUSE_MOTION, GObject_MouseOver);
	// Callbacks for game mode
	registerCallback(SDL_EVENT_KEYMAP_CHANGED, onKeyMapChanged);
}

bool GearX::eventFilter(SDL_Event* event) {
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		switch (event->type)
		{
		case SDL_EVENT_MOUSE_MOTION:
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
		case SDL_EVENT_MOUSE_WHEEL:
			return false;
		default:
			break;
		}
	}
	return true;
}