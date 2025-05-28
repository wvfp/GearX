#pragma once
#include "../../core/global/global.hpp"
namespace GearX {
	class ScriptHolder {
	private:
		sol::load_result _script;
		sol::state& lua;
	public:
		ScriptHolder(sol::state& _lua, const std::string& path) :lua(_lua) {
			_script = lua.load_file(path);
			sol::load_status s= _script.status();
			switch (s)
			{
			case sol::load_status::ok:
				std::cout << "Load OK : " << path<<std::endl;
				break;
			case sol::load_status::syntax:
				std::cout << "Syntax Error : " << path<<std::endl;
				break;
			case sol::load_status::memory:
				std::cout << "memory : " << path<<std::endl;
				break;
			case sol::load_status::gc:
				std::cout << "GC : " << path<<std::endl;
				break;
			case sol::load_status::file:
				std::cout << "File : " << path<<std::endl;
				break;
			default:
				break;
			}
		}
		~ScriptHolder() {
			_script.~load_result();
		}
		template<typename... Args>
		void execute(Args&&... args) {
	    // 包装，调用
			if (_script.valid()) {
				_script.call(std::forward<Args>(args)...);
			}
		}
		void reload(const std::string& path) {
			_script = lua.load_file(path);
			sol::load_status s = _script.status();
			switch (s)
			{
			case sol::load_status::ok:
				std::cout << "Load OK : " << path << std::endl;
				break;
			case sol::load_status::syntax:
				std::cout << "Syntax Error : " << path << std::endl;
				break;
			case sol::load_status::memory:
				std::cout << "memory : " << path << std::endl;
				break;
			case sol::load_status::gc:
				std::cout << "GC : " << path << std::endl;
				break;
			case sol::load_status::file:
				std::cout << "File : " << path << std::endl;
				break;
			default:
				break;
			}
		}
		sol::state& getLuaState() { return lua; };
		bool isRegister{ false };
	};
	extern  ScriptHolder* loadScript(std::string url);
}