#include "level.hpp"

namespace GearX {
	RTTR_REGISTRATION
	{
		rttr::registration::class_<Level>("Level")
		.property(u8"关卡路径",&Level::getLevelUrlRelative,&Level::setLevelUrlRelative)
		.property(u8"关卡名称",&Level::getLevelName,&Level::setLevelName)
		.property(u8"窗口大小",&Level::getWindowSize,&Level::setWindowSize)
		.property(u8"渲染纹理大小",&Level::getTargetTextureSize,&Level::setTargetTextureSize)
		.property(u8"渲染区域",&Level::getRenderRect,&Level::setRenderRect)
		.property(u8"重力",&Level::getGravity,&Level::setGravity)
		.property(u8"休眠",&Level::getAllowSleep,&Level::setAllowSleep)
		.property(u8"子步进",&Level::getSubStep,&Level::setSubStep)
		.property(u8"热启动",&Level::getWarmStarting,&Level::setWarmStarting);
	};
	void Level::setLevelURL(const std::string url) {
		if (!url.empty()) {
			namespace fs = std::filesystem;
			if (!m_level_url.empty()) {
				//之前的level文件
				save();
				fs::copy(m_level_url, m_level_url + ".bak");
			}
			if (fs::exists(url)) {
				static const std::regex jsonRegex(R"(\.(json)$)");
				if (std::regex_search(url, jsonRegex)) {
					load(url, true);
				} else{
					load(url, false);
				}
			}
			m_level_url = url;
		}
	}
	void GearX::Level::setLevelUrlRelative(const std::string url) {
		namespace fs = std::filesystem;
		if (!url.empty()) {
			std::string a_url = RuntimeGlobalContext::current_path.generic_string();
			a_url += "/Levels/" + url;
			setLevelURL(a_url);
		}
	}
	// 加载关卡
	bool Level::load(const std::string path, bool isJson) {
		std::ifstream ifs;
		if (!isJson)
			ifs.open(path, std::ios::binary);
		else
			ifs.open(path);
		if (ifs.is_open()) {
			if (!isJson) {
				cereal::PortableBinaryInputArchive archive(ifs);
				archive(*this);
			}
			else {
				cereal::JSONInputArchive archive(ifs);
				archive(*this);
			}
			m_level_url = path;
			initTextureSize();
			return true;
		}
		m_level_url = path;
		return false;
	}

	// 保存关卡
	void Level::save() {
		if (m_level_url.empty()) {
			//创建一个随机文件名
			std::string path = "Level_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".gxlevel";
			m_level_url = RuntimeGlobalContext::current_path.generic_string() + "/Levels/" + path;
		}
		static const std::regex jsonRegex(R"(\.(json)$)");
		if (std::regex_search(m_level_url, jsonRegex)) {
			std::ofstream ofs(m_level_url);
			if (ofs.is_open()) {
				cereal::JSONOutputArchive archive(ofs);
				archive(*this);
			}
		}
		else {
			std::ofstream ofs(m_level_url, std::ios::binary);
			if (ofs.is_open()) {
				cereal::PortableBinaryOutputArchive archive(ofs);
				archive(*this);
			}
		}
	}

	void GearX::Level::setListener(b2ContactListener* listener) {
		world.SetContactListener(listener);
	}

	// 另存为
	void Level::saveAs(const std::string& path) {
		if (path.empty()) return;
		m_level_url = path;
		save();
	}

	LevelObjectMap& Level::getAllObject() {
		return m_objects;
	}

	void GearX::Level::setGravity(std::array<float, 2> gravity) {
		world.SetGravity(b2Vec2(gravity[0], gravity[1]));
	}

	std::array<float, 2> GearX::Level::getGravity(void) {
		b2Vec2 gravity = world.GetGravity();
		return { gravity.x,gravity.y };
	}
	void Level::setRenderRect(std::array<float, 4> rect) {
		render_rect_size = rect;
	}
	std::array<float, 4> Level::getRenderRect() {
		return render_rect_size;
	}
	void  Level::setWarmStarting(bool ws) {
		world.SetWarmStarting(ws);
	}
	bool Level::getWarmStarting() {
		return world.GetWarmStarting();
	}
	void  Level::setSubStep(bool ss) {
		world.SetSubStepping(ss);
	}
	bool Level::getSubStep() {
		return world.GetSubStepping();
	}
	void  Level::setAllowSleep(bool as) {
		world.SetAllowSleeping(as);
	}
	bool Level::getAllowSleep() {
		return world.GetAllowSleeping();
	}
	void GearX::Level::initWindowSize(){
			
		SDL_SetWindowSize(RuntimeGlobalContext::SDL_CONTEXT.window, window_size[0], window_size[1]);
	}
	std::array<float, 2> GearX::Level::getWindowSize(){
		int w, h;
		SDL_GetWindowSize(RuntimeGlobalContext::SDL_CONTEXT.window, &w, &h);
		window_size[0] = w;
		window_size[1] = h;
		return window_size;
	}
	std::array<float, 2> GearX::Level::getTargetTextureSize() const
	{
		return tex_size;
	}

	void GearX::Level::setTargetTextureSize(std::array<float, 2> size) {
		if (size[0] < 1) {
			size[0] = 1;
		}
		if (size[1] < 1) {
			size[1] = 1;
		}
		tex_size = size;
		auto texture = SDL_CreateTexture(RuntimeGlobalContext::SDL_CONTEXT.renderer,
			SDL_GetWindowPixelFormat(RuntimeGlobalContext::SDL_CONTEXT.window),
			SDL_TEXTUREACCESS_TARGET, tex_size[0] * 8, tex_size[1] * 8);
		SDL_SetTextureScaleMode(texture, SDL_ScaleMode::SDL_SCALEMODE_LINEAR);
		std::swap(RuntimeGlobalContext::SDL_CONTEXT.texture, texture);
		SDL_DestroyTexture(texture);

	}

	// 添加对象
	void Level::addObject(std::shared_ptr<GObject> object) {
		if (!object) return;
		m_objects[object->getID()] = object;
		setObjectToLayer(object->getID(), m_current_layer);
		object->setLevel(getSharedPtr());
	}

	void GearX::Level::addObjectWithCopy(GObjectID id) {
		m_objects[id]->copy();
	}

	// 移除对象
	void Level::removeObject(std::shared_ptr<GObject> object) {
		if (!object) return;
		m_objects.erase(object->getID());
		removeObjectFromLayer(object->getID());
	}

	void Level::removeObject(GObjectID id) {
		if (m_objects.count(id)) {
			m_objects.erase(id);
			removeObjectFromLayer(id);
		}
	}

	// 创建新对象
	GObjectID Level::CreateObject(const std::string& name) {
		GObjectID min_valid_id = 1;
		for (GObjectID i = 1; i < invalid_id; i++) {
			if (m_objects.count(i) == 0) {
				min_valid_id = i;
				break;
			}
		}
		auto newObj = std::make_shared<GObject>(min_valid_id);
		newObj->setName(name);
		m_objects[newObj->getID()] = newObj;
		newObj->setLevel(getSharedPtr());
		setObjectToLayer(newObj->getID(), m_current_layer);
		return newObj->getID();
	}

	// 通过ID获取对象
	std::shared_ptr<GObject> Level::GetObjectByID(const GObjectID& id) {
		auto it = m_objects.find(id);
		return (it != m_objects.end()) ? it->second : nullptr;
	}

	// 获取关卡名称
	std::string Level::getLevelName() const {
		return m_level_name;
	}

	std::string GearX::Level::getLevelUrlRelative() {
		namespace fs = std::filesystem;
		return fs::path(m_level_url).filename().generic_string();
	}

	// 设置关卡名称
	void Level::setLevelName(const std::string name) {
		m_level_name = name;
	}

	// 获取关卡路径
	std::string Level::getLevelURL() const {
		return m_level_url;
	}

	LayerObjectTree& Level::getLayers() {
		return m_layers;
	}
	Uint16 Level::getCurrentLayer() {
		return m_current_layer;
	}
	void Level::setCurrentLayer(Uint16 layer) {
		if (layer < m_layers.size())
			m_current_layer = layer;
	}
	bool GearX::Level::getLayerFromID(GObjectID id, Uint16& layer) {
		for (int i = 0; i < m_layers.size(); i++) {
			auto it = std::find(m_layers[i].begin(), m_layers[i].end(), id);
			if (it != m_layers[i].end()) {
				layer = i;
				return true;
			}
		}
		return false;
	}
	void GearX::Level::setObjectToLayer(GObjectID id, Uint16 layer) {
		Uint16 _layer = 0;
		if (getLayerFromID(id, _layer) && _layer != layer) {
			removeObjectFromLayer(id);
		}
		if (layer < m_layers.size()) {
			m_layers[layer].push_back(id);
		}
	}
	void GearX::Level::removeObjectFromLayer(GObjectID id) {
		Uint16 _layer = 0;
		if (getLayerFromID(id, _layer)) {
			m_layers[_layer].erase(std::find(m_layers[_layer].begin(), m_layers[_layer].end(), id));
		}
	}
	void Level::createNewLayer() {
		m_layers.push_back(std::list<GObjectID>());
	}
	void Level::deleteLayer(Uint16 layer) {
		// 不删除第一层
		if (!layer)
			return;
		if (m_layers.size() > layer) {
			std::vector<GObjectID> ids;
			for (GObjectID i : m_layers.at(layer)) {
				ids.push_back(i);
			}
			for (auto i : ids)
				removeObject(i);
			m_layers.erase(m_layers.begin() + layer);
		}
	}
} // namespace GearX