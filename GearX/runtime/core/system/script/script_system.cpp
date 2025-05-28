#include "script_system.hpp"
#include "../audio/audio_system.hpp"
#include "../../global/global.hpp"
#include "../../framework/component/transform/transform_component.hpp"
#include "../../framework/component/texture/texture_component.hpp"
#include "../../framework/component/rigidbody/rigidbody_component.hpp"
#include "../../framework/component/script/script_component.hpp"
#include "../../framework/object/object.hpp"
#include "../../framework/level/level.hpp"
#include "../../framework/world/world.hpp"
#include "../../../resources/asset_manager/asset_manager.hpp"
#include "../../../resources/loader/script_loader.hpp"
#include "../../utils/object_wrapper.hpp"
#include "../../event/event.hpp"
namespace GearX {
	/// Called when two fixtures begin to touch.
	void  ContactListener::BeginContact(b2Contact* contact) {
		auto& script_system = GearX::RuntimeGlobalContext::scriptSystem;
		auto obj_A = getObjectbyFixture(contact->GetFixtureA());
		auto obj_B = getObjectbyFixture(contact->GetFixtureB());
		auto com = obj_A->getComponentByTypeName(rttr::type::get<GearX::ScriptComponent>().get_name());
		auto& lua = script_system.getLuaState();
		lua["ContactedObjectA"] = obj_A->getID();
		lua["ContactedObjectB"] = obj_B->getID();
		if (com) {
			auto script_com = std::dynamic_pointer_cast<GearX::ScriptComponent>(com);
			auto& scripts = script_com->getScriptBeginContact();
			for (auto& script : scripts) {
				if (script.data) {
					script_system.RegisterObjectAsSelf(obj_A->getID());
					
					static_cast<ScriptHolder*>(script.data)->execute();
				}
			}
		}
		com = obj_B->getComponentByTypeName(rttr::type::get<GearX::ScriptComponent>().get_name());
		if (com) {
			auto script_com = std::dynamic_pointer_cast<GearX::ScriptComponent>(com);
			auto& scripts = script_com->getScriptBeginContact();
			for (auto& script : scripts) {
				if (script.data) {
					script_system.RegisterObjectAsSelf(obj_B->getID());
					static_cast<ScriptHolder*>(script.data)->execute();
				}
			}
		}
	}

	/// Called when two fixtures cease to touch.
	void ContactListener::EndContact(b2Contact* contact) {
		auto& script_system = GearX::RuntimeGlobalContext::scriptSystem;
		auto obj_A = getObjectbyFixture(contact->GetFixtureA());
		auto obj_B = getObjectbyFixture(contact->GetFixtureB());
		auto com = obj_A->getComponentByTypeName(rttr::type::get<GearX::ScriptComponent>().get_name());
		auto& lua = script_system.getLuaState();
		lua["ContactedObjectA"] = obj_A->getID();
		lua["ContactedObjectB"] = obj_B->getID();
		if (com) {
			auto script_com = std::dynamic_pointer_cast<GearX::ScriptComponent>(com);
			auto& scripts = script_com->getScriptEndContact();
			for (auto& script : scripts) {
				if (script.data) {
					script_system.RegisterObjectAsSelf(obj_A->getID());
					static_cast<ScriptHolder*>(script.data)->execute();
				}
			}
		}
		com = obj_B->getComponentByTypeName(rttr::type::get<GearX::ScriptComponent>().get_name());
		if (com) {
			auto script_com = std::dynamic_pointer_cast<GearX::ScriptComponent>(com);
			auto& scripts = script_com->getScriptEndContact();
			for (auto& script : scripts) {
				if (script.data) {
					script_system.RegisterObjectAsSelf(obj_B->getID());
					static_cast<ScriptHolder*>(script.data)->execute();
				}
			}
		}
	}

	std::shared_ptr<GearX::GObject> ContactListener::getObjectbyFixture(b2Fixture* fixture) {
		auto& objs = GearX::RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
		for (auto& obj : objs) {
			if (obj.second->getFixture() == fixture) {
				return obj.second;
			}
		}
		return nullptr;
	}
}

GearX::ContactListener GearX::ScriptSystem::listener = GearX::ContactListener();

void GearX::ScriptSystem::RegisterEmptyLevelFuncToLua() {
	sol::table table = lua_state.create_table();
	// 注册Level中的函数到lua中
	table["getName"] = [&]() {
		return sol::nil;
		};
	table["setName"] = [&](std::string name) {
		return sol::nil;
		};
	table["getRenderRect"] = sol::as_table([&]() {
		return sol::nil;
		});
	table["setRenderRect"] = [&](const sol::table& rect) {
		};
	table["getGravity"] = [&]() {
		return sol::nil;
		};
	table["setGravity"] = [&](const sol::table& gravity) {
		};
	table["getTargetTextureSize"] = [&]() {
		return sol::nil;
		};
	table["setTargetTextureSize"] = [&](const sol::table& size) {
		return sol::nil;
		};
	table["createObject"] = [&](std::string name)->GObjectID {
		return 0;
	};
	lua_state["level"] = table;
}

GearX::ScriptSystem::ScriptSystem() :lua_state(RuntimeGlobalContext::lua) {
	lua_state.open_libraries(sol::lib::base, sol::lib::package,
		sol::lib::string, sol::lib::math,
		sol::lib::table, sol::lib::coroutine,
		sol::lib::debug, sol::lib::bit32);
	lua_state["KeyState"] = sol::nil;
	lua_state["KeyStateSize"] = sol::nil;
	//注册获取对象的函数
	lua_state.set_function("getObject", [this](GObjectID id) -> sol::table {return sol::nil; });
	//注册获取所有对象的函数
	lua_state.set_function("getAllObject", [this](GObjectID id) -> sol::table {return sol::nil; });
	RegisterEmptyLevelFuncToLua();
	//注册相关事件枚举到lua中(SDL_Event)
	GearX::Event::registerEventToLua(lua_state);
	RuntimeGlobalContext::audioSystem.RegisterToLua(lua_state);
	tables[0] = GObjectWrapper::getEmptyTable(lua_state);
	RegisterObjectAsSelf(0);
	lua_state["ContactedObjectA"] = sol::nil;
	lua_state["ContactedObjectB"] = sol::nil;
}

GearX::ScriptSystem::~ScriptSystem() {
	//因为（scriptAsser里）load_result这个类型与ScriptSystem中的lua有关联，
	//必须先析构load_result才可以析构lua,所以在这个析构函数中提前析构，防止出错
	auto& assets = GearX::RuntimeGlobalContext::assetManager.getAllAsset();
	for (auto& script : assets) {
		if (script.second.type == AssetType::Script) {
			GearX::RuntimeGlobalContext::assetManager.releaseAsset(script.first);
		}
	}
	tables.clear();
	TableOfObjects.clear();
	wrappers.clear();
}


void GearX::ScriptSystem::tick(float deltaTime) {

	auto level = RuntimeGlobalContext::world.getCurrentLevel();
	if (level) {
		if (!isInitialize) {
			level->getWorld().SetContactListener(&listener);
			RegisterTables();
			isInitialize = true;
		}
		auto& objs = level->getAllObject();
		for (auto& obj : objs) {
			auto& com = obj.second->getComponentByTypeName(rttr::type::get<ScriptComponent>().get_name());
			if (com) {
				//遍历脚本
				auto& script_com = std::dynamic_pointer_cast<ScriptComponent>(com);
				auto& scripts = script_com->getScript();
				for (auto& script : scripts) {
					if (script.data) {
						// 执行脚本
						RegisterObjectAsSelf(obj.first);
						static_cast<ScriptHolder*>(script.data)->execute();
					}
				}
			}
		}
	}
}

void GearX::ScriptSystem::RegisterCurrentLevelFuncToLua() {
	auto& level = RuntimeGlobalContext::world.getCurrentLevel();
	if (LevelFuncTables.count(level->getLevelURL()) == 0) {
		auto& table = lua_state.create_table();
		// 注册Level中的函数到lua中
		table["getName"] = [&](){
			return level->getLevelName();
		};
		table["setName"] = [&](std::string name) {
			level->setLevelName(name);
		};
		table["getRenderRect"] = sol::as_table([&](){
			return level->getRenderRect();
		});
		table["setRenderRect"] = [&](const sol::table& rect) {
			level->setRenderRect(cvtArray<4>(rect));
			};
		table["getGravity"] = [&](){
			return level->getGravity();
		};
		table["setGravity"] = [&](const sol::table& gravity) {
			level->setGravity(cvtArray<2>(gravity));
		};
		table["getTargetTextureSize"] = [&](){
			return level->getTargetTextureSize();
		};
		table["setTargetTextureSize"] = [&](const sol::table& size) {
			level->setTargetTextureSize(cvtArray<2>(size));
		};
		table["createObject"] = [&](std::string name)->GObjectID {
			auto obj = level->CreateObject(name);
			auto& object = level->GetObjectByID(obj);
			// 添加组件
			TransformComponent::addComponentTo(object);
			TextureComponent::addComponentTo(object);
			RigidBodyComponent::addComponentTo(object);
			ScriptComponent::addComponentTo(object);
			wrappers[obj] = std::make_shared<GObjectWrapper>(object);
			tables[obj] = wrappers[obj]->getTable(lua_state);
			return obj;
		};
		LevelFuncTables[level->getLevelURL()] = table;
	}
	lua_state["level"] = LevelFuncTables[level->getLevelURL()];
}

void GearX::ScriptSystem::RegisterTables(){
	//注册Level到lua中
	
	//注册相关枚举
	GObjectWrapper::RegisterEnum(lua_state);
	auto& Objs = RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
	for (auto& obj : Objs) {
		wrappers[obj.first] = std::make_shared<GObjectWrapper>(obj.second);
		tables[obj.first] = wrappers[obj.first]->getTable(lua_state);
	}
	//注册获取对象的函数
	lua_state.set_function("getObject", [this](GObjectID id) -> sol::table {
		if (tables.find(id) != tables.end()) {
			return	tables[id];
		}else{
			return sol::nil;
		}
	});
	//注册获取所有对象的函数
	lua_state.set_function("getAllObject", [this]() -> sol::table {
		sol::table all_objects = lua_state.create_table();
		for (const auto& pair : tables) {
			all_objects[pair.first] = pair.second;
		}
		return all_objects;
		});
}

void GearX::ScriptSystem::RegisterObjectAsSelf(GObjectID id){
	// 注册self为当前对象的表,便于脚本中直接使用self来访问对象属性和方法
	lua_state["Self"] = tables[id];
}
