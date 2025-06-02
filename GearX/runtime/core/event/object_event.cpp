#include "object_event.hpp"
#include "../framework/component/rigidbody/rigidbody_component.hpp"
#include "../framework/component/transform/transform_component.hpp"
#include "../framework/component/texture/texture_component.hpp"
#include "../framework/object/object.hpp"
#include "../framework/level/level.hpp"
const Uint32 EVENT_GOBJECT_ONSELECTED = SDL_RegisterEvents(1);
const Uint32 EVENT_GOBJECT_ONUNSELECTED = SDL_RegisterEvents(1);
const Uint32 EVENT_GOBJECT_ONPRESSED = SDL_RegisterEvents(1);
const Uint32 EVENT_GOBJECT_ONRELEASED = SDL_RegisterEvents(1);
const Uint32 EVENT_GOBJECT_ONDRAG = SDL_RegisterEvents(1);
const Uint32 EVENT_GOBJECT_ONSCALE = SDL_RegisterEvents(1);

// ��Ϸģʽ�µ�����¼�,�����ͣ,�ƶ�(�ڶ����ϵ�)���������ѹ���ͷ�
const Uint32 GEVENT_GOBJECT_MOUSEHOVER = SDL_RegisterEvents(1);
const Uint32 GEVENT_GOBJECT_MOUSEENTER = SDL_RegisterEvents(1);
const Uint32 GEVENT_GOBJECT_MOUSELEAVE = SDL_RegisterEvents(1);
const Uint32 GEVENT_GOBJECT_MOUSEMOVE = SDL_RegisterEvents(1);
const Uint32 GEVENT_GOBJECT_MOUSECLICK = SDL_RegisterEvents(1);
const Uint32 GEVENT_GOBJECT_MOUSEPRESS = SDL_RegisterEvents(1);
const Uint32 GEVENT_GOBJECT_MOUSERELEASE = SDL_RegisterEvents(1);
const Uint32 GEVENT_GOBJECT_MOUSEWHEEL = SDL_RegisterEvents(1);


GearX::GObjectID  GearX::SelectedObj = 0;
static bool isSelectedObjOnPressed = false;

// ѡ������,SDL_MOUSE_EVENT_DOWN
void GearX::GObject_Select(const SDL_Event& event) {
	if (GearX::RuntimeGlobalContext::isGameMode) {
		return;
	}
	bool isCon = false;
	if (event.button.button == SDL_BUTTON_LEFT && event.button.clicks == 2)
		isCon = true;
	if (!isCon)
		return;
	int w, h;
	SDL_GetWindowSize(RuntimeGlobalContext::SDL_CONTEXT.window, &w, &h);
	if (RuntimeGlobalContext::world.getCurrentLevel()) {
		auto& objs = RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
		auto& lvl = RuntimeGlobalContext::world.getCurrentLevel();
		//����ӳ��
		auto rect = lvl->getRenderRect();
		float b_x = event.button.x * rect[2] / (float)w;
		float b_y = event.button.y * rect[3] / (float)h;
		b_x += rect[0], b_y += rect[1];
		auto& layers = RuntimeGlobalContext::world.getCurrentLevel()->getLayers();
		for (auto layer = layers.rbegin(); layer!= layers.rend(); ++layer) {
			for (auto id_it = layer->rbegin(); id_it!= layer->rend(); ++id_it) {
				auto id = *id_it;
				auto obj = objs[id];
				auto com = obj->getComponentByTypeName(
					rttr::type::get<TransformComponent>().get_name());
				std::shared_ptr<TransformComponent> transformCom;
				std::shared_ptr<TextureComponent> textureCom;
				if (com)
					transformCom = std::dynamic_pointer_cast<TransformComponent>(com);
				else
					continue;
				com = obj->getComponentByTypeName(
					rttr::type::get<TextureComponent>().get_name());
				if (com)
					textureCom = std::dynamic_pointer_cast<TextureComponent>(com);
				else
					continue;
				// ��ȡĿ��Rect
				auto rect = textureCom->getDstRect();
				rect[0] = transformCom->getPositionX();
				rect[1] = transformCom->getPositionY();
				// �жϰ���λ���Ƿ���Ŀ������ཻ
				if (isPointInRectRotate({ b_x,b_y },
					rect, transformCom->getOrigin(), transformCom->getRotation())) {
					if (SelectedObj!= obj->getID() && objs.count(SelectedObj) != 0) {
						objs[SelectedObj]->setDrawAxis(false);
						SelectedObj = 0;
						SDL_Event u_event;
						u_event.type = EVENT_GOBJECT_ONUNSELECTED;
						u_event.user.type = EVENT_GOBJECT_ONUNSELECTED;
						u_event.user.code = SelectedObj;
						SDL_PushEvent(&u_event);
					}
					SelectedObj = obj->getID();
					obj->setDrawAxis(true);
					SDL_Event u_event;
					u_event.type = EVENT_GOBJECT_ONSELECTED;
					u_event.user.type = EVENT_GOBJECT_ONSELECTED;
					u_event.user.code = SelectedObj;
					SDL_PushEvent(&u_event);
					return;
				}
			
			}
		}
		if (SelectedObj) {
			objs[SelectedObj]->setDrawAxis(false);
			SelectedObj = 0;
			SDL_Event u_event;
			u_event.type = EVENT_GOBJECT_ONUNSELECTED;
			u_event.user.type = EVENT_GOBJECT_ONUNSELECTED;
			u_event.user.code = SelectedObj;
			SDL_PushEvent(&u_event);
		}
	}
}

// �϶�����,EVENT_MOUSE_MOTION
void GearX::GObject_Drag(const SDL_Event& event) {
	if (GearX::RuntimeGlobalContext::isGameMode) {
		return;
	}
	if (SelectedObj == 0 || !isSelectedObjOnPressed)
		return;
	int w = 0, h = 0;
	SDL_GetWindowSize(RuntimeGlobalContext::SDL_CONTEXT.window, &w, &h);
	if (SelectedObj != 0 && RuntimeGlobalContext::world.getCurrentLevel()) {
		auto& objs = RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
		//����ӳ��
		auto& lvl = RuntimeGlobalContext::world.getCurrentLevel();
		auto rect = lvl->getRenderRect();
		float b_x = event.motion.xrel * rect[2] / (float)w;
		float b_y = event.motion.yrel * rect[3] / (float)h;
		if (objs.count(SelectedObj) != 0) {
			auto obj = objs[SelectedObj];
			if (obj) {
				auto com = obj->getComponentByTypeName(
					rttr::type::get<TransformComponent>().get_name());
				std::shared_ptr<TransformComponent> transformCom;
				std::shared_ptr<TextureComponent> textureCom;
				{
					if (com)
						transformCom = std::dynamic_pointer_cast<TransformComponent>(com);
					else
						return;
					com = obj->getComponentByTypeName(
						rttr::type::get<TextureComponent>().get_name());
					if (com)
						textureCom = std::dynamic_pointer_cast<TextureComponent>(com);
					else
						return;
				}
				// ��ȡĿ��Position
				auto pos = transformCom->getPosition();
				pos[0] += b_x;
				pos[1] += b_y;
				transformCom->setPosition(pos);
				SDL_Event u_event;
				u_event.type = EVENT_GOBJECT_ONDRAG;
				u_event.user.type = EVENT_GOBJECT_ONDRAG;
				u_event.user.code = SelectedObj;
				SDL_PushEvent(&u_event);
			}
		}
	}
}

// ���������ϣ�SDL_MOUSE_EVENT_DOWN
void GearX::GObject_Press(const SDL_Event& event) {
	if (GearX::RuntimeGlobalContext::isGameMode) {
		return;
	}
	bool isCon = false;
	if (event.button.button == SDL_BUTTON_LEFT && event.button.clicks == 1)
		isCon = true;
	if (!isCon && SelectedObj == 0)
		return;
	int w, h;
	SDL_GetWindowSize(RuntimeGlobalContext::SDL_CONTEXT.window, &w, &h);
	if (RuntimeGlobalContext::world.getCurrentLevel()) {
		auto& objs = RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
		auto& lvl = RuntimeGlobalContext::world.getCurrentLevel();
		//����ӳ��
		auto rect = lvl->getRenderRect();
		float b_x = event.button.x * rect[2] / (float)w;
		float b_y = event.button.y * rect[3] / (float)h;
		b_x += rect[0], b_y += rect[1];
		if (objs.count(SelectedObj) != 0) {
			auto obj = objs[SelectedObj];
			if (obj) {
				auto com = obj->getComponentByTypeName(
					rttr::type::get<TransformComponent>().get_name());
				std::shared_ptr<TransformComponent> transformCom;
				std::shared_ptr<TextureComponent> textureCom;
				{
					if (com)
						transformCom = std::dynamic_pointer_cast<TransformComponent>(com);
					else
						return;
					com = obj->getComponentByTypeName(
						rttr::type::get<TextureComponent>().get_name());
					if (com)
						textureCom = std::dynamic_pointer_cast<TextureComponent>(com);
					else
						return;
				}
				// ��ȡĿ��Rect
				auto rect = textureCom->getDstRect();
				// �жϰ���λ���Ƿ���Ŀ������ཻ
				if (isPointInRectRotate({ b_x,b_y },
					rect, transformCom->getOrigin(), transformCom->getRotation())) {
					isSelectedObjOnPressed = true;
					// �����¼�
					SDL_Event u_event;
					u_event.type = EVENT_GOBJECT_ONPRESSED;
					u_event.user.type = EVENT_GOBJECT_ONPRESSED;
					u_event.user.code = SelectedObj;
					SDL_PushEvent(&u_event);
				}
			}
		}
	}
}

// �ͷ���꣬SDL_MOUSE_EVENT_UP
void GearX::GObject_Release(const SDL_Event& event) {
	if (GearX::RuntimeGlobalContext::isGameMode) {
		return;
	}
	if (isSelectedObjOnPressed && event.button.button == SDL_BUTTON_LEFT)
		isSelectedObjOnPressed = false;
}
// ���Ŷ���
void GearX::GObject_Scale(const SDL_Event& event) {
	if (GearX::RuntimeGlobalContext::isGameMode) {
		return;
	}
	if (SelectedObj == 0)
		return;
	int w = 0, h = 0;
	SDL_GetWindowSize(RuntimeGlobalContext::SDL_CONTEXT.window, &w, &h);
	if (SelectedObj != 0 && RuntimeGlobalContext::world.getCurrentLevel()) {
		auto& objs = RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
		//����ӳ��
		auto& lvl = RuntimeGlobalContext::world.getCurrentLevel();
		auto rect = lvl->getRenderRect();
		float b_x = event.wheel.mouse_x * rect[2] / (float)w;
		float b_y = event.wheel.mouse_y * rect[3] / (float)h;
		b_x += rect[0], b_y += rect[1];
		if (objs.count(SelectedObj) != 0) {
			auto& obj = objs[SelectedObj];
			if (obj) {
				auto com = obj->getComponentByTypeName(
					rttr::type::get<TransformComponent>().get_name());
				std::shared_ptr<TransformComponent> transformCom;
				std::shared_ptr<TextureComponent> textureCom;
				{
					if (com)
						transformCom = std::dynamic_pointer_cast<TransformComponent>(com);
					else
						return;
					com = obj->getComponentByTypeName(
						rttr::type::get<TextureComponent>().get_name());
					if (com)
						textureCom = std::dynamic_pointer_cast<TextureComponent>(com);
					else
						return;
				}
				// ��ȡĿ��Rect
				auto rect = textureCom->getDstRect();
				// �ж����λ���Ƿ���Ŀ������ཻ
				if (isPointInRectRotate({ b_x,b_y },
					rect, transformCom->getOrigin(), transformCom->getRotation())) {
					auto scale = transformCom->getScale();
					scale[0] += event.wheel.y * 0.01;
					scale[1] += event.wheel.y * 0.01;
					transformCom->setScale(scale);
					SDL_Event u_event;
					u_event.type = EVENT_GOBJECT_ONDRAG;
					u_event.user.type = EVENT_GOBJECT_ONDRAG;
					u_event.user.code = SelectedObj;
					SDL_PushEvent(&u_event);
				}
			}
		}
	}
}

// �жϵ��Ƿ�����ת������
bool GearX::isPointInRectRotate(std::array<float, 2> point,
	std::array<float, 4> rect,
	std::array<float, 2> center,
	float angle) {
	// ��ȡ���β���
	float x = rect[0];
	float y = rect[1];
	float width = rect[2];
	float height = rect[3];

	// �����������
	float centerX = x + width / 2.0f;
	float centerY = y + height / 2.0f;

	// ��������ת�����Ծ�������Ϊԭ�������ϵ
	float px = point[0] - centerX;
	float py = point[1] - centerY;

	// ������ת�����Һ�����ֵ
	float rad = -angle * PI / 180.0f;
	float cosAngle = std::cos(rad);
	float sinAngle = std::sin(rad);

	// ������ת�ؾ���δ��תʱ��״̬
	float rotatedX = px * cosAngle - py * sinAngle;
	float rotatedY = px * sinAngle + py * cosAngle;

	// �����Ƿ��ھ��α߽���
	return (rotatedX >= -width / 2.0f && rotatedX <= width / 2.0f &&
		rotatedY >= -height / 2.0f && rotatedY <= height / 2.0f);
}
// Hover��־
static GearX::GObjectID Object_Hover_ID = 0;
// �������������
void GearX::GObject_MouseHover(const SDL_Event& event){
	int w, h;
	SDL_GetWindowSize(RuntimeGlobalContext::SDL_CONTEXT.window, &w, &h);
	if (RuntimeGlobalContext::world.getCurrentLevel()) {
		auto& objs = RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
		auto& lvl = RuntimeGlobalContext::world.getCurrentLevel();
		//����ӳ��
		auto rect = lvl->getRenderRect();
		float b_x = event.button.x * rect[2] / (float)w;
		float b_y = event.button.y * rect[3] / (float)h;
		b_x += rect[0], b_y += rect[1];
		auto& layers = RuntimeGlobalContext::world.getCurrentLevel()->getLayers();
		for (auto layer = layers.rbegin(); layer != layers.rend(); ++layer) {
			for (auto id_it = layer->rbegin(); id_it != layer->rend(); ++id_it) {
				auto id = *id_it;
				auto obj = objs[id];
				auto com = obj->getComponentByTypeName(
					rttr::type::get<TransformComponent>().get_name());
				std::shared_ptr<TransformComponent> transformCom;
				std::shared_ptr<TextureComponent> textureCom;
				if (com)
					transformCom = std::dynamic_pointer_cast<TransformComponent>(com);
				else
					continue;
				com = obj->getComponentByTypeName(
					rttr::type::get<TextureComponent>().get_name());
				if (com)
					textureCom = std::dynamic_pointer_cast<TextureComponent>(com);
				else
					continue;
				// ��ȡĿ��Rect
				auto rect = textureCom->getDstRect();
				// �ж�λ���Ƿ���Ŀ������ཻ
				if (isPointInRectRotate({ b_x,b_y },
					rect, transformCom->getOrigin(), transformCom->getRotation())) {
					Object_Hover_ID = id;
					SDL_Event *u_event =	new SDL_Event;
					static ObjectEvent object_event;
					u_event->type = GEVENT_GOBJECT_MOUSEHOVER;
					u_event->user.type = GEVENT_GOBJECT_MOUSEHOVER;
					u_event->user.code = GEVENT_GOBJECT_MOUSEHOVER;
					object_event.id = id;
					object_event.x = event.motion.x;
					object_event.y = event.motion.y;
					object_event.xrel = event.motion.xrel;
					object_event.yrel = event.motion.yrel;
					object_event.button = event.motion.state;
					object_event.timestamp = event.motion.timestamp - SDL_GetTicksNS();
					u_event->user.data1 = reinterpret_cast<void*>(&object_event);
					SDL_PushEvent(u_event);
					delete u_event;
					return;
				}
			}
		}
	}
}

// �¼�Hover����
void GearX::GObject_MouseEnter(const SDL_Event& event){
	static GearX::GObjectID curent_object_hover_id = 0;
	static ObjectEvent object_event;
	// �ж���굱ǰ�����Ƿ�����һ�ε����岻ͬ
	if (Object_Hover_ID != 0 && curent_object_hover_id != Object_Hover_ID) {
		curent_object_hover_id = Object_Hover_ID;
		SDL_Event *u_event = new SDL_Event;
		u_event->type = GEVENT_GOBJECT_MOUSEENTER;
		u_event->user.type = GEVENT_GOBJECT_MOUSEENTER;
		u_event->user.code = GEVENT_GOBJECT_MOUSEENTER;
		object_event = *(ObjectEvent*)event.user.data1;
		object_event.id = Object_Hover_ID;
		u_event->user.data1 = reinterpret_cast<void*>(&object_event);
		SDL_PushEvent(u_event);
		delete u_event;
	}
}
// �����Hover����
void GearX::GObject_MouseLeave(const SDL_Event& event){
	static GearX::GObjectID curent_object_hover_id = 0;
	static ObjectEvent object_event;
	// �ж���굱ǰ�����Ƿ�����һ�ε����岻ͬ
	if (Object_Hover_ID != 0 && curent_object_hover_id != Object_Hover_ID) {
		SDL_Event *u_event = new SDL_Event;
		u_event->type = GEVENT_GOBJECT_MOUSELEAVE;
		u_event->user.type = GEVENT_GOBJECT_MOUSELEAVE;
		u_event->user.code = GEVENT_GOBJECT_MOUSELEAVE;
		object_event = *(ObjectEvent*)event.user.data1;
		object_event.id = curent_object_hover_id;
		curent_object_hover_id = Object_Hover_ID;
		u_event->user.data1 = reinterpret_cast<void*>(&object_event);
		SDL_PushEvent(u_event);
		delete u_event;
	}
	
}
// ������ƶ�����SDL_EVEN	T_MOUSEMOTION
void GearX::GObject_MouseMove(const SDL_Event& event){
	static ObjectEvent object_event;
	if (Object_Hover_ID != 0) {
		SDL_Event *u_event = new SDL_Event;
		u_event->type = GEVENT_GOBJECT_MOUSEMOVE;
		u_event->user.type = GEVENT_GOBJECT_MOUSEMOVE;
		u_event->user.code = GEVENT_GOBJECT_MOUSEMOVE;
		object_event.id = Object_Hover_ID;
		object_event.x = event.motion.x;
		object_event.y = event.motion.y;
		object_event.xrel = event.motion.xrel;
		object_event.yrel = event.motion.yrel;
		object_event.button = event.motion.state;
		object_event.timestamp = event.motion.timestamp;
		u_event->user.data1 = reinterpret_cast<void*>(&object_event);
		SDL_PushEvent(u_event);
		delete u_event;
	}
}
// ���������
void GearX::GObject_MouseClick(const SDL_Event& event){
	static ObjectEvent object_event;
	if (Object_Hover_ID != 0) {
		SDL_Event *u_event = new SDL_Event;
		u_event->type = GEVENT_GOBJECT_MOUSECLICK;
		u_event->user.type = GEVENT_GOBJECT_MOUSECLICK;
		u_event->user.code = GEVENT_GOBJECT_MOUSECLICK;
		object_event.id = Object_Hover_ID;
		object_event.x = event.button.x;
		object_event.y = event.button.y;
		object_event.button = event.button.button;
		object_event.clicks = event.button.clicks;
		object_event.timestamp = event.button.timestamp;
		u_event->user.data1 = reinterpret_cast<void*>(&object_event);
		SDL_PushEvent(u_event);
	}
}
// ��갴�´���
static GearX::GObjectID curent_press_object_id = 0;
void GearX::GObject_MousePress(const SDL_Event& event){
	static ObjectEvent object_event;
	if (Object_Hover_ID != 0 && curent_press_object_id != Object_Hover_ID) {
		curent_press_object_id = Object_Hover_ID;
		SDL_Event *u_event = new SDL_Event;
		u_event->type = GEVENT_GOBJECT_MOUSEPRESS;
		u_event->user.code = GEVENT_GOBJECT_MOUSEPRESS;
		u_event->user.type = GEVENT_GOBJECT_MOUSEPRESS;
		object_event.id = Object_Hover_ID;
		object_event.x = event.button.x;
		object_event.y = event.button.y;
		object_event.button = event.button.button;
		object_event.clicks = event.button.clicks;
		object_event.timestamp = event.button.timestamp;
		u_event->user.data1 = reinterpret_cast<void*>(&object_event);
		SDL_PushEvent(u_event);
		delete u_event;
	}
}
// ����ͷŴ���
void GearX::GObject_MouseRelease(const SDL_Event& event){
	static ObjectEvent object_event;
	if (Object_Hover_ID != 0 && curent_press_object_id != 0) {
		SDL_Event *u_event = new SDL_Event;
		u_event->type = GEVENT_GOBJECT_MOUSERELEASE;
		u_event->user.type = GEVENT_GOBJECT_MOUSERELEASE;
		u_event->user.code = GEVENT_GOBJECT_MOUSERELEASE;
		object_event.id = Object_Hover_ID;
		object_event.x = event.button.x;
		object_event.y = event.button.y;
		object_event.button = event.button.button;
		object_event.clicks = event.button.clicks;
		object_event.timestamp = event.button.timestamp;
		u_event->user.data1 = reinterpret_cast<void*>(&object_event);
		SDL_PushEvent(u_event);
		delete u_event;
		curent_press_object_id = 0;
	}
}
// �����ִ���
void GearX::GObject_MouseWheel(const SDL_Event& event){
	static ObjectEvent event_wheel;
	if (Object_Hover_ID != 0) {
		SDL_Event *u_event = new SDL_Event;
		u_event->type = GEVENT_GOBJECT_MOUSEWHEEL;
		u_event->user.type = GEVENT_GOBJECT_MOUSEWHEEL;
		u_event->user.code = GEVENT_GOBJECT_MOUSEWHEEL;
		event_wheel.id = Object_Hover_ID;
		event_wheel.x = event.wheel.mouse_x;
		event_wheel.y = event.wheel.mouse_y;
		event_wheel.direction = event.wheel.direction;
		event_wheel.xrel = event.wheel.x;
		event_wheel.yrel = event.wheel.y;
		event_wheel.timestamp = event.wheel.timestamp;
		u_event->user.data1 = reinterpret_cast<void*>(&event_wheel);
		SDL_PushEvent(u_event);
		delete u_event;
	}
}
