#pragma once
#include "../system.hpp"
#include "../../framework/object/object.hpp"
namespace GearX {
	class GObject;
	class GObjectWrapper;
	class ScriptHolder;
	class ContactListener;
	class ScriptHolder;
	class ContactListener : public b2ContactListener {
	public:
		/// Called when two fixtures begin to touch.
		void BeginContact(b2Contact* contact);
		/// Called when two fixtures cease to touch.
		void EndContact(b2Contact* contact);
		std::shared_ptr<GearX::GObject> getObjectbyFixture(b2Fixture* fixture);
	};
	// The script system is responsible for loading and executing lua scripts.
	class ScriptSystem : public System {
	public:
		using ObjectDataTable = std::map<GObjectID, sol::table>;
		using ObjectWrapperMap = std::map<GObjectID, std::shared_ptr<GObjectWrapper>>;
		ScriptSystem();
		~ScriptSystem();
		void tick(float deltaTime) override;
		void reset() { isInitialize = false; }
		void RegisterTables();
		void RegisterObjectAsSelf(GObjectID id);
		void RegisterObjectAsSelf(GObjectID id, sol::environment& env);
		void RegisterCurrentLevelFuncToLua();
		void RegisterEmptyLevelFuncToLua();
		sol::environment& getLuaEnv() { return *env; }
		sol::state& getLuaState() { return lua_state; }
	private:
		bool isInitialize = false;
		sol::state& lua_state;
		std::unique_ptr<sol::environment> env;
		ObjectDataTable tables;
		ObjectWrapperMap wrappers;
		std::map<std::string, sol::table> LevelFuncTables;
		static GearX::ContactListener listener;
	};
}
