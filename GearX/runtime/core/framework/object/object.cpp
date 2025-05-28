#include "object.hpp"
#include "../component/rigidbody/rigidbody_component.hpp"
namespace GearX {
	// 静态成员初始化
	GObjectID GObject::m_next_id = 1; // 从1开始分配
	Uint32 invalid_id = std::numeric_limits<Uint32>::max();
	// 构造函数 ID ,只能由Level创建
	GObject::GObject(GObjectID id) {
		m_id = id;
	}
	// 构造函数
	GObject::GObject() : m_id(m_next_id++) {
		// 保证ID有效性
		if (m_next_id == invalid_id) m_next_id = 1;
	}
	GObject::~GObject() {
		for (auto component : m_components) {
			component.reset();
		}
		m_components.clear();
	}

	// 获取共享指针
	std::shared_ptr<GObject> GObject::getSharedPtr() {
		return shared_from_this();
	}

	// 获取名称
	const std::string& GObject::getName() const {
		return m_name;
	}

	// 设置名称
	void GObject::setName(const std::string& name) {
		m_name = name;
	}

	// 获取ID
	GObjectID GObject::getID() const {
		return m_id;
	}

	// 设置ID（谨慎使用）
	void GObject::setID(GObjectID id) {
		m_id = id;
		if (m_next_id <= id) m_next_id = id + 1; // 更新ID生成器
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

	// 获取所有组件
	const std::vector<std::shared_ptr<Component>>& GObject::getAllComponents() const {
		return m_components;
	}

	// 通过类型名查找组件
	std::shared_ptr<Component> GObject::getComponentByTypeName(const std::string& typeName) {
		for (auto& comp : m_components) {
			if (rttr::type::get(*comp).get_name() == typeName) {
				return comp;
			}
		}
		return nullptr;
	}

	// 添加组件
	void GObject::addComponent(std::shared_ptr<Component> component) {
		if (!component) return;
		component->setParentObject(shared_from_this()); // 假设Component有setParentObject
		m_components.push_back(component);
	}

	// 移除组件
	void GObject::removeComponent(std::shared_ptr<Component> component) {
		auto it = std::remove(m_components.begin(), m_components.end(), component);
		m_components.erase(it, m_components.end());
	}

	// 设置所属关卡
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

	// 获取所属关卡
	std::shared_ptr<Level> GObject::getLevel() const {
		return parent_level.lock();
	}
} // namespace GearX