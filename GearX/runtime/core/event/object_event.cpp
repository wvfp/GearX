#include "object_event.hpp"
#include "../framework/component/rigidbody/rigidbody_component.hpp"
#include "../framework/component/transform/transform_component.hpp"
#include "../framework/component/texture/texture_component.hpp"
#include "../framework/object/object.hpp"
#include "../framework/level/level.hpp"

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
		for (auto& obj : objs) {
			if (obj.second) {
				auto com = obj.second->getComponentByTypeName(
					rttr::type::get<TransformComponent>().get_name());
				std::shared_ptr<TransformComponent> transformCom;
				std::shared_ptr<TextureComponent> textureCom;
				if (com)
					transformCom = std::dynamic_pointer_cast<TransformComponent>(com);
				else
					continue;
				com = obj.second->getComponentByTypeName(
					rttr::type::get<TextureComponent>().get_name());
				if (com)
					textureCom = std::dynamic_pointer_cast<TextureComponent>(com);
				else
					continue;
				// ��ȡĿ��Rect
				auto rect = textureCom->getDstRect();
				// �жϰ���λ���Ƿ���Ŀ������ཻ
				if (isPointInRectRotate({ b_x,b_y },
					rect, transformCom->getOrigin(), transformCom->getRotation())) {
					if (objs.count(SelectedObj) != 0) {
						objs[SelectedObj]->setDrawAxis(false);
					}
					SelectedObj = obj.first;
					obj.second->setDrawAxis(true);
					SDL_Event u_event;
					u_event.type = EVENT_GOBJECT_ONSELECTED;
					u_event.user.type = EVENT_GOBJECT_ONSELECTED;
					u_event.user.code = SelectedObj;
					SDL_PushEvent(&u_event);
				}
				else {
					if (obj.second && SelectedObj == obj.first) {
						objs[SelectedObj]->setDrawAxis(false);
					}
					else
						continue;
					SDL_Event u_event;
					u_event.type = EVENT_GOBJECT_ONUNSELECTED;
					u_event.user.type = EVENT_GOBJECT_ONUNSELECTED;
					u_event.user.code = SelectedObj;
					SelectedObj = 0;
				}
			}
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
				// ��ȡĿ��Rect
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
// �����������
void GearX::GObject_MouseOver(const SDL_Event& event){
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
		for (auto& obj : objs) {
			if (obj.second) {
				auto com = obj.second->getComponentByTypeName(
					rttr::type::get<TransformComponent>().get_name());
				std::shared_ptr<TransformComponent> transformCom;
				std::shared_ptr<TextureComponent> textureCom;
				if (com)
					transformCom = std::dynamic_pointer_cast<TransformComponent>(com);
				else
					continue;
				com = obj.second->getComponentByTypeName(
					rttr::type::get<TextureComponent>().get_name());
				if (com)
					textureCom = std::dynamic_pointer_cast<TextureComponent>(com);
				else
					continue;
				// ��ȡĿ��Rect
				auto rect = textureCom->getDstRect();
				// �жϰ���λ���Ƿ���Ŀ������ཻ
				if (isPointInRectRotate({ b_x,b_y },
					rect, transformCom->getOrigin(), transformCom->getRotation())) {
					SDL_Event u_event;
					u_event.type = EVENT_GOBJECT_ONMOUSEOVER;
					u_event.user.type = EVENT_GOBJECT_ONMOUSEOVER;
					u_event.user.code = obj.first;
					SDL_PushEvent(&u_event);
				}
			}
		}
	}
}
