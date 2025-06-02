#include "editor.hpp"
#include "../utils/render_axis.hpp"
#include "../event/object_event.hpp"
#include "../framework/component/transform/transform_component.hpp"
#include "../framework/component/texture/texture_component.hpp"
#include "../framework/component/rigidbody/rigidbody_component.hpp"
#include "../framework/component/script/script_component.hpp"
namespace GearX {
	namespace fs = std::filesystem;

	// 文件条目结构
	struct FileEntry {
		fs::path path;         // 完整路径
		std::string name{ "" };      // 显示名称
		bool is_directory;     // 是否是目录
		size_t size;           // 文件大小（字节）
		std::string extension{ "" }; // 文件扩展名
	};
	// 文件浏览器状态
	struct FileBrowserState {
		fs::path current_path;          // 当前路径
		std::vector<FileEntry> entries; // 当前目录条目
		int selected_entry = -1;       // 选中条目索引
		bool show_editor = false;       // 是否显示编辑器
		char* edit_content = nullptr;       // 使用动态分配的 char 数组
		size_t edit_capacity = 0;           // 缓冲区总容量
		uint32_t text_length = 0;           // 实际文本长度
		fs::path editing_file;          // 正在编辑的文件
		bool modified = false;          // 是否修改未保存
		float font_scale = 1.0f; // 添加字体缩放系数
	};
	// 添加光标状态结构
	//函数前置声明
	void RenderFileBrowser(FileBrowserState& state);
	int TextEditCallback(ImGuiInputTextCallbackData* data);
	// 释放内存
	void CleanupFileBrowser(FileBrowserState& state) {
		if (state.edit_content) {
			delete[] state.edit_content;
			state.edit_content = nullptr;
			state.edit_capacity = 0;
			state.text_length = 0;
		}
	}
	int TextEditCallback(ImGuiInputTextCallbackData* data) {
		FileBrowserState* state = static_cast<FileBrowserState*>(data->UserData);
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			// 计算需要的新容量（至少 2 倍增长策略）
			const size_t required_capacity = data->BufTextLen + 1;
			if (required_capacity > state->edit_capacity) {
				const size_t new_capacity = required_capacity * 2;
				char* new_buf = new char[new_capacity];
				memset(new_buf, '\0', new_capacity);
				// 复制旧数据
				memcpy(new_buf, state->edit_content, state->text_length);
				new_buf[data->BufTextLen] = '\0';
				// 释放旧内存
				delete[] state->edit_content;

				// 更新状态
				state->edit_content = new_buf;
				state->edit_capacity = new_capacity;
			}

			// 更新缓冲区指针
			data->Buf = state->edit_content;
		}

		return 0;
	}

	// 初始化文件内容
	bool InitFile(FileBrowserState& state, std::ifstream& file) {
		if (file) {
			const size_t file_size = file.tellg();
			file.seekg(0);

			// 释放旧内存（如果存在）
			if (state.edit_content) delete[] state.edit_content;

			// 分配新内存（+1 用于终止符）
			state.edit_content = new char[file_size + 1];
			memset(state.edit_content, '\0', file_size + 1);
			state.edit_capacity = file_size + 1;
			file.read(state.edit_content, file_size);

			state.edit_content[file_size] = '\0';
			state.text_length = file_size;
			state.show_editor = true;
			state.modified = false;
			return true;
		}
		return false;
	}

	bool SaveFile(FileBrowserState& state) {
		if (state.editing_file.empty()) return false;

		try {
			std::ofstream file(state.editing_file);
			if (file) {
				file.write(state.edit_content, state.edit_capacity);
				state.modified = false;
				file.close();
				return true;
			}
		}
		catch (const std::exception& e) {
			// 处理保存失败
			ImGui::OpenPopup("Save Error");
		}
		return false;
	}
	bool ReloadFile(FileBrowserState& state) {
		std::ifstream ifs(state.editing_file, std::ios::ate);
		return InitFile(state, ifs);
	}
	// 初始化文件浏览器
	void InitFileBrowser(FileBrowserState& state) {
		if (state.current_path.empty())
			state.current_path = fs::current_path();
		fs::directory_iterator it(state.current_path);
		state.entries.clear();
		// 添加返回上级目录
		if (state.current_path.has_parent_path()) {
			state.entries.push_back({
				state.current_path.parent_path(),
				"..",
				true,
				0,
				""
				});
		}

		// 遍历目录
		for (const auto& entry : fs::directory_iterator(state.current_path)) {
			FileEntry fe;
			fe.path = entry.path();
			fe.name = entry.path().filename().string();
			fe.is_directory = entry.is_directory();
			fe.extension = entry.path().extension().string();
			fe.size = entry.is_directory() ? 0 : entry.file_size();

			state.entries.push_back(fe);
		}
	}
	// 2. 修改编辑器渲染函数
	void RenderEditor(FileBrowserState& state) {
		ImGui::Begin("Editor", &state.show_editor, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking);
		// 应用字体缩放（必须在所有控件创建前调用）
		ImGui::SetWindowFontScale(state.font_scale);

		// 菜单栏
		if (ImGui::BeginMenuBar()) {
			// 文件菜单
			if (ImGui::BeginMenu(u8"文件")) {
				if (ImGui::MenuItem(u8"保存")) {
					SaveFile(state);
				}
				if (ImGui::MenuItem(u8"重新加载")) {
					ReloadFile(state);
				}
				ImGui::EndMenu();
			}

			// 新增视图菜单
			if (ImGui::BeginMenu(u8"视图")) {
				// 快捷按钮
				if (ImGui::MenuItem(u8"放大字体")) {
					state.font_scale = ImMin(state.font_scale + 0.1f, 3.0f);
				}
				if (ImGui::MenuItem(u8"缩小字体")) {
					state.font_scale = ImMax(state.font_scale - 0.1f, 0.5f);
				}

				ImGui::EndMenu();
			}

			// 状态指示器（保持原有位置）
			ImGui::TextColored(state.modified ? ImVec4(1, 0.2f, 0.2f, 1) : ImVec4(0.2f, 1, 0.2f, 1),
				state.modified ? u8"已修改" : u8"已保存");
			ImGui::EndMenuBar();
		}

		// 计算可用区域（自动考虑缩放）
		ImVec2 avail_size = ImGui::GetContentRegionAvail();

		// 调整输入框高度适应字体
		const float font_size = ImGui::GetTextLineHeight();
		if (avail_size.y < font_size * 5) {
			avail_size.y = font_size * 5; // 保持最小高度
		}

		// 文本编辑区域
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput |
			ImGuiInputTextFlags_CallbackCharFilter |
			ImGuiInputTextFlags_CallbackResize;

		if (ImGui::InputTextMultiline("##content",
			state.edit_content,
			state.edit_capacity,
			avail_size,
			flags,
			TextEditCallback,
			&state))
		{
			state.modified = true;
		}

		ImGui::End();
	}

	// 3. 在初始化时加载保存的字体设置
	void LoadEditorSettings(FileBrowserState& state) {
		// 从INI文件读取（示例使用ImGui默认INI）
		state.font_scale = ImGui::GetIO().FontDefault->Scale; // 初始值
		if (ImGui::GetIO().IniFilename) {
			// 需要自定义存储（ImGui默认INI不直接支持自定义值）
			// 这里示例使用简单实现
			std::ifstream ini(ImGui::GetIO().IniFilename);
			std::string line;
			while (std::getline(ini, line)) {
				if (sscanf(line.c_str(), "FontScale=%f", &state.font_scale) == 1) {
					break;
				}
			}
		}
	}

	// 4. 在退出时保存设置
	void SaveEditorSettings(const FileBrowserState& state) {
		if (ImGui::GetIO().IniFilename) {
			std::ofstream ini(ImGui::GetIO().IniFilename, std::ios::app);
			ini << "\nFontScale=" << state.font_scale << "\n";
		}
	}
	// 文件浏览器窗口
	void RenderFileBrowser(FileBrowserState& state) {
		ImGui::Begin(u8"文件浏览器", nullptr, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar()) {
			// 目录操作按钮
			if (ImGui::MenuItem(u8"返回")) { // 返回上级目录
				if (state.current_path.has_parent_path()) {
					state.current_path = state.current_path.parent_path();
					InitFileBrowser(state);
				}
			}
			if (ImGui::MenuItem(u8"刷新")) { // 刷新目录
				InitFileBrowser(state);
			}
			// 路径导航栏
			ImGui::Text("Path: %s", state.current_path.parent_path().filename().c_str(),
				"/", state.current_path.c_str());
			ImGui::EndMenuBar();
		}

		// 文件列表
		static bool isPathModify{ false };
		static fs::path ModifiedPath;
		ImVec2 avail_size = ImGui::GetContentRegionAvail();
		if (ImGui::BeginListBox("##Files", avail_size)) {
			if (state.entries.empty()) {
				ImGui::Text(u8"空目录");
			}
			else {
				ImGuiListClipper clipper;
				clipper.Begin(state.entries.size(), ImGui::GetTextLineHeightWithSpacing());

				while (clipper.Step()) {
					for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
						const auto& entry = state.entries[i];
						ImGui::PushID(i);

						std::string type;
						std::string stus;
						type = entry.is_directory ? u8"[目录] " : u8"[文件] ";
						// 可选项
						bool selected = (i == state.selected_entry);
						if (ImGui::Selectable(("##" + type + entry.name + stus).c_str(), selected)) {
							state.selected_entry = i;
							if (entry.is_directory) {
								isPathModify = true;
								ModifiedPath = entry.path;
							}
							else if (entry.extension == ".txt" || entry.extension == ".lua") {
								// 打开文本文件
								std::ifstream file(entry.path, std::ios::ate);
								InitFile(state, file);
								state.editing_file = entry.path.c_str();
								file.close();
							}
							//}
						}
						ImGui::SameLine();
						ImGui::TextColored(entry.is_directory ?
							ImVec4(0.8f, 0.8f, 0.4f, 0.8f) : ImVec4(0.8f, 0.8f, 0.8f, 0.8f),
							u8"%s", type.c_str());
						ImGui::SameLine();
						if (!entry.is_directory) {
							stus = entry.extension + "\t" + std::to_string(entry.size / 1024.0f)
								+ " kb";
						}
						if (entry.is_directory) {
							ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.4f, 0.8f),
								u8"%s", entry.name.c_str());
						}
						else {
							ImGui::TextColored(entry.extension == ".lua" ?
								ImVec4(0.5, 0.9, 0.4, 1.0f) : ImVec4(0.9f, 0.9f, 0.9f, 0.9f),
								u8"%s\t%s\t%0.2f %s", entry.name.c_str(), entry.extension.c_str(),
								entry.size / 1024.0f, "kb");
						}
						ImGui::PopID();
					}
				}
			}
			ImGui::EndListBox();
		}
		ImGui::End();
		if (isPathModify) {
			state.current_path = ModifiedPath;
			InitFileBrowser(state);
		}
	}
	static FileBrowserState filebrowserstate;
	static bool isBroswerInit{ false };
}

GearX::GObjectID currentObj = 0;
namespace GearX {
	static std::map < std::string, std::vector<char>> buf;
	template<class Type>
	void showPropEditor(rttr::property& prop, Type& com) {
		ImGui::Text(u8"属性: %s", prop.get_name().c_str());
		if (prop.is_readonly()) {
			ImGui::Text(u8"值: %s", prop.get_value(com).to_string().c_str());
			return;
		}
		else if (prop.get_type().is_enumeration()) {
			auto e = prop.get_enumeration();
			ImGui::PushID(prop.get_name().c_str());
			if (ImGui::BeginCombo("", prop.get_value(com).to_string().c_str())) {
				for (auto i : e.get_values()) {
					if (ImGui::Selectable(i.to_string().c_str())) {
						prop.set_value(com, i);
					}
				}
				ImGui::EndCombo();
			}
			ImGui::PopID();
		}
		else {
			if (prop.get_type() == rttr::type::get<float>()) {
				float f = prop.get_value(com).to_float();
				ImGui::PushID(prop.get_name().c_str());
				if (ImGui::DragFloat("", &f, 0.01f, 0.0f, 0.0f, "%.2f")) {
					prop.set_value(com, f);
				}
				ImGui::PopID();
			}
			else if (prop.get_type() == rttr::type::get<int>()) {
				int i = prop.get_value(com).to_int();
				ImGui::PushID(prop.get_name().c_str());
				if (ImGui::DragInt("", &i)) {
					prop.set_value(com, i);
				}
				ImGui::PopID();
			}
			else if (prop.get_type() == rttr::type::get<bool>()) {
				bool b = prop.get_value(com).to_bool();
				ImGui::PushID(prop.get_name().c_str());
				if (ImGui::BeginCombo("##BoolSelect", prop.get_value(com).to_string().c_str())) {
					if (ImGui::Selectable("true")) {
						prop.set_value(com, true);
					}
					if (ImGui::Selectable("false")) {
						prop.set_value(com, false);
					}
					ImGui::EndCombo();
				}
				ImGui::PopID();
			}
			else if (prop.get_type() == rttr::type::get<std::string>()) {
				char* data = nullptr;
				if (buf.count(prop.get_name()) == 0) {
					buf[prop.get_name()] = std::vector<char>(256, 0);
					data = buf[prop.get_name()].data();
					std::strcpy(data, prop.get_value(com).get_value<std::string>().c_str());
				}
				if (data == nullptr) {
					data = buf[prop.get_name()].data();
				}
				ImGui::PushID(prop.get_name().c_str());
				ImGui::InputText("", data, 256);
				ImGui::PopID();
				ImGui::PushID((prop.get_name() + std::string(u8"确定")).c_str());
				if (ImGui::Button(u8"确定")) {
					prop.set_value(com, std::string(data));
				}
				ImGui::SameLine();
				if (ImGui::Button(u8"刷新")) {
					std::strcpy(data, prop.get_value(com).to_string().c_str());
				}
				ImGui::PopID();
			}
			else if (prop.get_type() == rttr::type::get<std::pair<float, float>>()) {
				auto pair = prop.get_value(com).get_value<std::pair<float, float>>();
				ImGui::Text(u8"值: ( %0.3f , %0.3f ) ", pair.first, pair.second);
			}
			else if (prop.get_type() == rttr::type::get<std::pair<int, int>>()) {
				auto pair = prop.get_value(com).get_value<std::pair<int, int>>();
				ImGui::Text(u8"值: ( %d , %d ) ", pair.first, pair.second);
			}
			else if (prop.get_type() == rttr::type::get < std::array<float, 4>>()) {
				std::array<float, 4> f = prop.get_value(com).get_value<std::array<float, 4>>();
				ImGui::PushID(prop.get_name().c_str());
				if (ImGui::DragFloat4("", f.data(), 0.01f, 0, 0, "%.2f")) {
					prop.set_value(com, f);
				}
				ImGui::PopID();
			}
			else if (prop.get_type() == rttr::type::get < std::array<float, 3>>()) {
				std::array<float, 3> f = prop.get_value(com).get_value<std::array<float, 3>>();
				ImGui::PushID(prop.get_name().c_str());
				if (ImGui::DragFloat3("", f.data(), 0.01f, 0, 0, "%.2f")) {
					prop.set_value(com, f);
				}
				ImGui::PopID();
			}
			else if (prop.get_type() == rttr::type::get < std::array<float, 2>>()) {
				std::array<float, 2> f = prop.get_value(com).get_value<std::array<float, 2>>();
				ImGui::PushID(prop.get_name().c_str());
				if (ImGui::DragFloat2("", f.data(), 0.01f, 0, 0, "%.2f")) {
					prop.set_value(com, f);
				}
				ImGui::PopID();
			}
			else if (prop.get_type() == rttr::type::get<ColorType>()) {
				auto color = prop.get_value(com).get_value<ColorType>();
				ImGui::PushID(prop.get_name().c_str());
				if (ImGui::ColorEdit4("", color.color.data(), ImGuiColorEditFlags_Float)) {
					prop.set_value(com, color);
				}
				ImGui::PopID();
			}
		}
	}
	static void showScriptComponent(std::shared_ptr<ScriptComponent> com) {
		if (ImGui::Button(u8"刷新脚本")) {
			com->reloadScripts();
		}
		auto obj = com->getParentObject();
		static int selectBlock = 0;
		static std::vector<std::string> block = {
			u8"脚本:游戏开始",u8"脚本:循环调用",u8"脚本:当碰撞开始",u8"脚本:当碰撞结束"
		};
		if(ImGui::BeginCombo("##SelectBlock",block[selectBlock].c_str())) {
			if (ImGui::Selectable(block[0].c_str())) {
				selectBlock = 0;
			}
			if (ImGui::Selectable(block[1].c_str())) {
				selectBlock = 1;
			}
			if (ImGui::Selectable(block[2].c_str())) {
				selectBlock = 2;
			}
			if (ImGui::Selectable(block[3].c_str())) {
				selectBlock = 3;
			}
			ImGui::EndCombo();
		}
		static auto scripts = com->getScript();
		switch (selectBlock) {
			case 0:
				scripts = com->getScriptDoOnce();
				break;
			case 1:
				scripts = com->getScript();
				break;
			case 2:
				scripts = com->getScriptBeginContact();
				break;
			case 3:
				scripts = com->getScriptEndContact();
		}
		static bool add = false;
		if (ImGui::Button(u8"添加脚本")) {
			add = true;
		}
		if (add) {
			ImGui::SameLine();
			if (ImGui::Button(u8"取消")) {
					add = false;
			}
			static std::array<char, 255> buf{ 0,0 };
			ImGui::InputText("##ScriptUrlInput", buf.data(), buf.size());
			if (ImGui::Button(u8"添加")) {
				std::string url(buf.data());
				static const std::regex Regex(R"(\.lua$)");
				if (!std::regex_search(url, Regex)) {
					url += ".lua";
				}
				if (!fs::exists(fs::path(RuntimeGlobalContext::current_path.generic_string() + "/Scripts"))) {
					fs::create_directory(fs::path(RuntimeGlobalContext::current_path.generic_string() + "/Scripts"));
				}
				auto s_url = fs::path(RuntimeGlobalContext::current_path.generic_string() + "/Scripts/" + url);
				s_url = fs::relative(s_url);
				if (!fs::exists(s_url)) {
					std::ofstream ofs(s_url, std::ios::ate);
					ofs.close();
				}
				switch (selectBlock){
					case 0:
						com->addScriptToDoOnce(s_url.generic_string());
						break;
					case 1:
						com->addScript(s_url.generic_string());
						break;
					case 2:
						com->addScriptToBeginContact(s_url.generic_string());
						break;
					case 3:
						com->addScriptToEndContact(s_url.generic_string());
						break;
				default:
					break;
				}
			}
		}

		if (obj) {
			static  Asset select;
			namespace fs = std::filesystem;
			if (ImGui::BeginCombo(("##Scripts_" + obj->getName() +
				std::to_string(obj->getID())).c_str(),
				fs::path(select.asset_url).filename().generic_string().c_str())) {
				for (auto& script : scripts) {
					if (ImGui::Selectable(fs::path(script.asset_url).filename()
						.generic_string().c_str())) {
						select = script;
					}
				}
				ImGui::EndCombo();
			}
			if (!select.asset_url.empty()) {
				if (ImGui::Button(u8"打开文件")) {
					std::ifstream ifs(select.asset_url, std::ios::ate);
					if (ifs.is_open()) {
						filebrowserstate.editing_file = fs::path(select.asset_url);
						InitFile(filebrowserstate, ifs);
						filebrowserstate.show_editor = true;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button(u8"重新加载")) {
					RuntimeGlobalContext::assetManager.reloadAssetScript(select.asset_url);
				}
				ImGui::SameLine();
				if (ImGui::Button(u8"移除")) {
					switch (selectBlock) {
						case 0:
							com->removeScriptFromDoOnce(select.asset_url);
							break;
						case 1:
							com->removeScript(select.asset_url);
							break;
						case 2:
							com->removeScriptFromBeginContect(select.asset_url);
							break;
						case 3:
							com->removeScriptFromEndContect(select.asset_url);
							break;
					}
					select = Asset();
				}
			}
		}
	}
	static void showLevelEditor() {
		auto level = RuntimeGlobalContext::world.getCurrentLevel();
		std::string levelUrl;
		std::string nowUrl;
		if (level) {
			ImGui::Checkbox(u8"游戏模式", &RuntimeGlobalContext::isGameMode);
			ImGui::SameLine();
			if (ImGui::Button(u8"更新脚本")) {
				// 脚本热更新
				auto objs = level->getAllObject();
				for (auto& i : objs) {
					auto com = i.second->getComponentByTypeName(rttr::type::get<ScriptComponent>().get_name());
					auto script = std::static_pointer_cast<ScriptComponent>(com);
					if (script) {
						script->reloadScripts();
					}
				}
			}
			static auto oldlevel = level;
			levelUrl = level->getLevelURL();
			auto  props = rttr::type::get<Level>().get_properties();
			for (auto& prop : props) {
				showPropEditor(prop, level);
				if (oldlevel != level) {
					std::strcpy(buf[u8"关卡路径"].data(), level->getLevelUrlRelative().c_str());
					std::strcpy(buf[u8"关卡名称"].data(), level->getLevelName().c_str());
					oldlevel = level;
				}
			}
			nowUrl = level->getLevelURL();
		}
		if (levelUrl != nowUrl) {
			RuntimeGlobalContext::world.reloadLevel(levelUrl);
			RuntimeGlobalContext::world.setCurrentLevel(nowUrl);
		}
	}
	static void showWorldEditor() {
		auto& world = RuntimeGlobalContext::world;
		ImGui::Text(u8"当前世界: %s", world.getWorldUrl().c_str());
		static char* buf = new char[256] {0, };
		ImGui::PushID("World_Url_Edit");
		ImGui::InputText("##World_Url_Edit", buf, 256);
		ImGui::PopID();
		if (ImGui::Button(u8"加载世界")) {
			std::string url(buf);
			filebrowserstate.current_path = fs::relative("./" + url).parent_path();
			RuntimeGlobalContext::current_path = filebrowserstate.current_path;
			url = fs::relative(url).generic_string();
			if (!fs::exists(RuntimeGlobalContext::current_path.generic_string() + "/Levels")) {
				fs::create_directory(RuntimeGlobalContext::current_path.generic_string() + "/Levels");
			}
			World tmp = World();
			tmp.loadWorld(url);
			world.clear();
			world = tmp;
			InitFileBrowser(filebrowserstate);
			isBroswerInit = true;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"保存世界")) {
			std::string url(buf);
			url = fs::relative(url).generic_string();
			world.saveWorld(url);
		}
		std::string levelurl = u8"无";
		if (!world.getCurrentLevelUrl().empty())
			levelurl = std::string(world.getCurrentLevelUrl());
		ImGui::Text(u8"当前关卡: %s", levelurl.c_str());
		if (ImGui::BeginCombo("##选择栏", levelurl.c_str())) {
			for (auto& i : world.getLevelsUrls()) {
				ImGui::PushID(i.empty() ? u8"##无" : i.c_str());
				if (ImGui::Selectable(fs::path(i).filename().generic_string().c_str())) {
					world.setCurrentLevel(i);
					currentObj = 0;
				}
				ImGui::PopID();
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button(u8"新建关卡")) {
			static std::mt19937 gen(SDL_GetTicks());
			std::uniform_int_distribution<> dis(0, 32480);
			world.setCurrentLevel(world.createLevel(
				RuntimeGlobalContext::current_path.generic_string() + "/Levels/" + std::string("NewLevel")
				+ "." + std::to_string(dis(gen)) + ".json"));
			currentObj = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"删除关卡")) {
			world.unloadLevel(world.getCurrentLevelUrl());
			currentObj = 0;
		}
	}
	static void showObjectEditor() {
		auto types = rttr::type::get<RigidBodyComponent>();
		auto types1 = rttr::type::get<TextureComponent>();
		auto str1 = types.get_name();
		auto str2 = types1.get_name();
		if (RuntimeGlobalContext::world.getCurrentLevel()) {
			auto& level = RuntimeGlobalContext::world.getCurrentLevel();
			auto& objs = RuntimeGlobalContext::world.getCurrentLevel()->getAllObject();
			currentObj = SelectedObj;
			if (currentObj && objs.count(currentObj) == 0) {
				currentObj = 0;
				SelectedObj = 0;
			}
			if (ImGui::Button(u8"新建对象")) {
				RuntimeGlobalContext::world.getCurrentLevel()->CreateObject("None");
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"删除对象")) {
				if (SelectedObj && SelectedObj == currentObj) {
					objs[currentObj]->setDrawAxis(false);
					SelectedObj = 0;
				}
				RuntimeGlobalContext::world.getCurrentLevel()->removeObject(currentObj);
				currentObj = 0;
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"拷贝对象")) {
				if (currentObj != 0)
					RuntimeGlobalContext::world.getCurrentLevel()->addObjectWithCopy(currentObj);
			}
			static char* buf = new char[256] {0, };
			ImGui::PushID("Obj");
			if (ImGui::BeginCombo("",
				currentObj != 0 && !objs[currentObj]->getName().empty() ?
				(objs[currentObj]->getName()).c_str() : std::string("None").c_str(), ImGuiComboFlags_HeightSmall)) {
				//Obj
				for (auto i : objs) {
					try {
						Uint16  layer;
						level->getLayerFromID(i.first, layer);
						ImGui::PushID(i.first);
						if (ImGui::Selectable(i.second->getName() == std::string() ? "None" : (i.second->getName() + std::string("\tID:") + std::to_string(i.first) + std::string("\tlayer:") + std::to_string(layer)).c_str())) {
							if (currentObj != 0)
								objs[currentObj]->setDrawAxis(false);
							currentObj = i.first;
							SelectedObj = currentObj;
							if (i.first != 0)
								objs[currentObj]->setDrawAxis(true);
							level->setCurrentLayer(layer);
						}
						ImGui::PopID();
					}
					catch (std::exception& e) {
						SDL_Log("Exception:%s", e.what());
					}
				}
				ImGui::EndCombo();
			}
			ImGui::PopID();
			if (ImGui::Button(u8"新建层")) {
				if (level) {
					level->createNewLayer();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"删除层")) {
				if (level) {
					level->deleteLayer(level->getCurrentLayer());
				}
			}
			if (ImGui::TreeNode("##TreeNodeLayer", "Layer")) {
				auto& layers = level->getLayers();
				for (int i = 0; i < layers.size(); i++) {
					if (ImGui::TreeNode(("##" + std::to_string(i)).c_str(), "Layer %d", i)) {
						level->setCurrentLayer(i);
						for (auto& j : layers[i]) {
							Uint16  layer;
							level->getLayerFromID(j, layer);
							if (ImGui::Selectable(objs[j]->getName() == std::string() ? "None" : (objs[j]->getName() +
								std::string("\tID:") + std::to_string(j)).c_str())) {
								if (currentObj != 0)
									objs[currentObj]->setDrawAxis(false);
								currentObj = j;
								SelectedObj = currentObj;
								if (currentObj != 0)
									objs[currentObj]->setDrawAxis(true);
							}
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
			if (currentObj == 0) {
				SelectedObj = 0;
				return;
			}
			//std::strcpy(buf, currentObj ? objs[currentObj]->getName().c_str() : "None:0");
			ImGui::Text(u8"对象名");
			ImGui::PushID("Obj_Name");
			ImGui::InputText("", buf, 256);
			ImGui::PopID();
			if (ImGui::Button(u8"确定更改")) {
				objs[currentObj]->setName(buf);
			}
			if (currentObj) {
				static rttr::type t = rttr::type::get<Component>();
				static rttr::type select_type = t;
				ImGui::Text(u8"组件:");
				if (ImGui::BeginCombo("##ComponentAdd", CCMap[select_type.get_name()].c_str())) {
					for (const auto& type : rttr::type::get_types()) {
						if (!type.is_pointer() && type.is_derived_from<Component>() && type != t) {
							if (ImGui::Selectable(CCMap[type.get_name()].c_str())) {
								select_type = type;
							}
						}
					}
					ImGui::EndCombo();
				}
				if (ImGui::Button(u8"添加组件")) {
					std::shared_ptr<Component> coms =
						objs[currentObj]->getComponentByTypeName(select_type.get_name());
					if (!coms) {
						auto ms = select_type.get_methods();
						rttr::method m = select_type.get_method("addComponentTo");
						// {}占位符，静态成员函数不需要对象
						if (m.is_valid())
							m.invoke({}, objs[currentObj]);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button(u8"删除组件")) {
					std::shared_ptr<Component> coms =
						objs[currentObj]->getComponentByTypeName(select_type.get_name());
					if (coms) {
						objs[currentObj]->removeComponent(coms);
					}
				}
				ImGui::Text(u8"当前组件:");
				for (auto& i : objs[currentObj]->getAllComponents()) {
					rttr::type t = rttr::type::get(*i);
					ImGui::Separator();
					std::string str = CCMap[t.get_name()];
					if (ImGui::CollapsingHeader(str.c_str())) {
						ImGui::Spacing();
						if (t == rttr::type::get<ScriptComponent>()) {
							auto com = std::dynamic_pointer_cast<ScriptComponent>(i);
							showScriptComponent(com);
						}
						for (auto& prop : t.get_properties()) {
							showPropEditor(prop, *i);
						}
					}
				}
			}
		}
	}
}

namespace GearX {
	void ObjectEditor::render() {
		// ImGUI Render
		{
			ImGui::Begin(u8"世界");
			ImGui::Separator();
			showWorldEditor();
			ImGui::Separator();
			ImGui::End();
		}
		{
			if (isBroswerInit) {
				RenderFileBrowser(filebrowserstate);
			}
			if (filebrowserstate.show_editor)
				RenderEditor(filebrowserstate);
			else
				CleanupFileBrowser(filebrowserstate);
		}
		{
			ImGui::Begin(u8"关卡");
			ImGui::Separator();
			showLevelEditor();
			ImGui::Separator();
			ImGui::End();
		}
		{
			ImGui::Begin(u8"对象");
			ImGui::Separator();
			showObjectEditor();
			ImGui::Separator();
			ImGui::End();
		}
	}
}