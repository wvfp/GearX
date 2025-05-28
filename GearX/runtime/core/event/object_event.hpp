#pragma once
#include "event.hpp"

#define PI 3.1415926535f

// ����ļ�����GObject(��Ϸ����)���¼��Լ��������϶������� ...��
// ��ѡ�еĶ����ID Ĭ��Ϊ0����δѡ��
const int EVENT_GOBJECT_ONSELECTED = SDL_RegisterEvents(1);
const int EVENT_GOBJECT_ONUNSELECTED = SDL_RegisterEvents(1);
const int EVENT_GOBJECT_ONPRESSED = SDL_RegisterEvents(1);
const int EVENT_GOBJECT_ONRELEASED = SDL_RegisterEvents(1);
const int EVENT_GOBJECT_ONDRAG = SDL_RegisterEvents(1);
const int EVENT_GOBJECT_ONSCALE = SDL_RegisterEvents(1);
//������ڶ�����ʱ�������¼�
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