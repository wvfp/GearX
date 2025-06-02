#pragma once
#include "event.hpp"

#define PI 3.1415926535f




namespace GearX {
	extern GObjectID  SelectedObj;
	extern bool isPointInRectRotate(std::array<float, 2> point, std::array<float, 4> rect,
		std::array<float, 2>center, float angle);
	// 游戏模式的鼠标事件处理，
	//包括鼠标悬停,移动(在对象上的)，点击，按压，释放
	void GObject_MouseHover(const SDL_Event& event);
	void GObject_MouseEnter(const SDL_Event& event);
	void GObject_MouseLeave(const SDL_Event& event);
	void GObject_MouseMove(const SDL_Event& event);
	void GObject_MouseClick(const SDL_Event& event);
	void GObject_MousePress(const SDL_Event& event);
	void GObject_MouseRelease(const SDL_Event& event);
	void GObject_MouseWheel(const SDL_Event& event);
	// 编辑模式的鼠标事件处理
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