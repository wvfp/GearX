#pragma once
#include "../../../depencies.hpp"
#include "../object/object.hpp"
#include "../../global/global.hpp"

namespace GearX {
	// Game  Object
	class GObject;
	extern std::map<std::string, std::string> CCMap;
	//  组件基类
	class Component : public std::enable_shared_from_this<Component> {
	private:
		RTTR_ENABLE()
			RTTR_REGISTRATION_FRIEND
	public:
		std::shared_ptr<Component> getSharedPtr() {
			return shared_from_this();
		}
		virtual std::shared_ptr<Component> copy()=0;
		template<class Archive>
		void serialize(Archive& archive) {
			archive(
				cereal::make_nvp("TypeName", m_type_name)
			);
		}
		virtual ~Component() = default;
		void setParentObject(std::shared_ptr<GObject> parent) {
			m_parentObject = parent;
		}
		const std::shared_ptr<GObject> getParentObject() const{
			return m_parentObject.lock();
		}
		bool isDirty() const {
			return m_isDirty;
		}
		void setDirty(bool dirty) {
			m_isDirty = dirty;
		}
		std::string getTypeName() const {
			return m_type_name;
		}
	protected:
		std::weak_ptr<GObject> m_parentObject;
		bool m_isDirty{ true };
		std::string m_type_name = "Component";
	};
}
CEREAL_REGISTER_TYPE(GearX::Component)