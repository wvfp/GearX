#pragma once
#include "event.hpp"

#define PI 3.1415926535f

// 这个文件放置GObject(游戏对象)的事件以及操作（拖动，缩放 ...）
// 被选中的对象的ID 默认为0，即未选中
const int EVENT_GOBJECT_ONSELECTED = SDL_RegisterEvents(1);
const int EVENT_GOBJECT_ONUNSELECTED = SDL_RegisterEvents(1);
const int EVENT_GOBJECT_ONPRESSED = SDL_RegisterEvents(1);
const int EVENT_GOBJECT_ONRELEASED = SDL_RegisterEvents(1);
const int EVENT_GOBJECT_ONDRAG = SDL_RegisterEvents(1);
const int EVENT_GOBJECT_ONSCALE = SDL_RegisterEvents(1);
//当鼠标在对象上时触发的事件
const int EVENT_GOBJECT_ONMOUSEOVER = SDL_RegisterEvents(1);


namespace GearX {
	extern GObjectID  SelectedObj;
	extern bool isPointInRectRotate(std::array<float, 2> point, std::array<float, 4> rect,
		std::array<float, 2>center, float angle);
	void GObject_MouseOver(const SDL_Event& event);
	void GObject_Select(const SDL_Event& event);
	void GObject_Press(const SDL_Event& event);
	void GObject_Drag(const SDL_Event& event);
	void GObject_Release(const SDL_Event& event);
	bool isPointInRectRotate(std::array<float, 2> point,
		std::array<float, 4> rect,
		std::array<float, 2> center,
		float angle);

	void GObject_Scale(const SDL_Event& event);
}