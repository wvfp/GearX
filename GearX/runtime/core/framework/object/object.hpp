#pragma once
#include "../../../depencies.hpp"
#include "../component/component.hpp"
#include "../level/level.hpp"
namespace GearX {
	class Level;
	class Component;
	using GObjectID = Uint32;
	extern Uint32 invalid_id;
	class GObject :public std::enable_shared_from_this<GObject> {
	public:
		GObject();
		GObject(GObjectID id);
		~GObject();
		std::shared_ptr<GObject> getSharedPtr();
		const std::string& getName() const;
		void setName(const std::string& name);
		GObjectID getID()const;
		void setID(GObjectID id);
		bool isDrawAxis()const { return m_isDrawAxis; }
		void setDrawAxis(bool draw = false) { m_isDrawAxis = draw; }
		b2Fixture* getFixture();
		const std::vector<std::shared_ptr<Component>>& getAllComponents() const;
		std::shared_ptr<Component> getComponentByTypeName(const std::string& typeName);
		void addComponent(std::shared_ptr<Component> component);
		void removeComponent(std::shared_ptr<Component> component);
		void setLevel(std::shared_ptr<Level> level);
        std::shared_ptr<GObject> copy() const;
		std::shared_ptr<Level> getLevel() const;
		template<typename Archive>
		void serialize(Archive& ar)
		{
			ar(
				cereal::make_nvp("Object_Name", m_name)
			);
			ar(
				cereal::make_nvp("Object_Component", m_components)
			);
			ar(
				cereal::make_nvp("Object_ID", m_id)
			);
			if (typeid(Archive) == typeid(cereal::JSONInputArchive) ||
				typeid(Archive) == typeid(cereal::PortableBinaryInputArchive)) {
				for (auto& com : m_components) {
					com->setParentObject(getSharedPtr());
				}
			}
		}
	private:
		GObjectID m_id = invalid_id;
		bool m_isDrawAxis = false;
		std::string m_name;
		std::vector <std::shared_ptr<Component>> m_components;
		static GObjectID m_next_id;
		std::weak_ptr<Level> parent_level;
	};
}