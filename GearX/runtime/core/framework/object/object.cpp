#include "object.hpp"
#include "../component/rigidbody/rigidbody_component.hpp"
namespace GearX {
	// ��̬��Ա��ʼ��
	GObjectID GObject::m_next_id = 1; // ��1��ʼ����
	Uint32 invalid_id = std::numeric_limits<Uint32>::max();
	// ���캯�� ID ,ֻ����Level����
	GObject::GObject(GObjectID id) {
		m_id = id;
	}
	// ���캯��
	GObject::GObject() : m_id(m_next_id++) {
		// ��֤ID��Ч��
		if (m_next_id == invalid_id) m_next_id = 1;
	}
	GObject::~GObject() {
		for (auto component : m_components) {
			component.reset();
		}
		m_components.clear();
	}

	// ��ȡ����ָ��
	std::shared_ptr<GObject> GObject::getSharedPtr() {
		return shared_from_this();
	}

	// ��ȡ����
	const std::string& GObject::getName() const {
		return m_name;
	}

	// ��������
	void GObject::setName(const std::string& name) {
		m_name = name;
	}

	// ��ȡID
	GObjectID GObject::getID() const {
		return m_id;
	}

	// ����ID������ʹ�ã�
	void GObject::setID(GObjectID id) {
		m_id = id;
		if (m_next_id <= id) m_next_id = id + 1; // ����ID������
	}

	b2Fixture* GObject::getFixture(){
		auto& component = getComponentByTypeName(rttr::type::get<RigidBodyComponent>().get_name());
		if (component) {
			return std::dynamic_pointer_cast<RigidBodyComponent>(component)->getFixture();
		}
		else {
			return nullptr;
		}
	}

	// ��ȡ�������
	const std::vector<std::shared_ptr<Component>>& GObject::getAllComponents() const {
		return m_components;
	}

	// ͨ���������������
	std::shared_ptr<Component> GObject::getComponentByTypeName(const std::string& typeName) {
		for (auto& comp : m_components) {
			if (rttr::type::get(*comp).get_name() == typeName) {
				return comp;
			}
		}
		return nullptr;
	}

	// ������
	void GObject::addComponent(std::shared_ptr<Component> component) {
		if (!component) return;
		component->setParentObject(shared_from_this()); // ����Component��setParentObject
		m_components.push_back(component);
	}

	// �Ƴ����
	void GObject::removeComponent(std::shared_ptr<Component> component) {
		auto it = std::remove(m_components.begin(), m_components.end(), component);
		m_components.erase(it, m_components.end());
	}

	// ���������ؿ�
	void GObject::setLevel(std::shared_ptr<Level> level) {
		parent_level = level;
	}

	std::shared_ptr<GObject> GObject::copy() const {
		if (parent_level.lock()) {
			GObjectID id = parent_level.lock()->CreateObject(m_name);
			auto& obj = parent_level.lock()->getAllObject()[id];
			for (auto& comp : m_components) {
				auto com = comp->copy();
				obj->addComponent(comp->copy());
			}
			return obj;
		}
		return nullptr;
	}

	// ��ȡ�����ؿ�
	std::shared_ptr<Level> GObject::getLevel() const {
		return parent_level.lock();
	}
} // namespace GearX