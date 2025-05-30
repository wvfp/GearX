#pragma once
#include "../../global/global.hpp"
#include "../object/object.hpp"
namespace GearX {
	class GObject;
	using GObjectID = Uint32;
	using LevelObjectMap = std::map<GObjectID, std::shared_ptr<GObject>>;
	using LayerObjectTree = std::vector<std::list<GObjectID>>;
	class Level : public std::enable_shared_from_this<Level> {
	public:
		Level() :world(b2Vec2(0.0f, 9.8f)),
			tex_size(std::array<float, 2>({ 1280.0f,960.0f })),
			window_size(std::array<float, 2>({ 1280.0f,960.0f })),
			render_rect_size(std::array<float, 4>({ 0.0f,0.0f,1280.0f,960.0f })),
			m_current_layer(0), m_layers(std::vector<std::list<GObjectID>>()) {
			m_layers.push_back(std::list<GObjectID>());
			initTextureSize();
		};
		~Level() {};
		bool load(const std::string path, bool isJson = false);
		void save();
		void setListener(b2ContactListener*);
		void saveAs(const std::string& path);
		void addObject(std::shared_ptr<GObject> object);
		void addObjectWithCopy(GObjectID id);
		void removeObject(std::shared_ptr<GObject> object);
		void removeObject(GObjectID id);
		GObjectID CreateObject(const std::string& name);
		std::shared_ptr<GObject> GetObjectByID(const GObjectID& id);
		std::string getLevelName() const;
		void setLevelURL(const std::string url);
		void setLevelUrlRelative(const std::string url);
		std::string getLevelUrlRelative();
		void setLevelName(const std::string name);
		std::string getLevelURL() const;
		LevelObjectMap& getAllObject();

		// b2World prop setter/getter
		void setGravity(std::array<float, 2> gravity);
		std::array<float, 2> getGravity();
		void setRenderRect(std::array<float, 4> rect);
		std::array<float, 4> getRenderRect();

		void  setWarmStarting(bool);
		bool getWarmStarting();
		void  setSubStep(bool);
		bool getSubStep();
		void  setAllowSleep(bool);
		bool getAllowSleep();
		void createNewLayer();
		void deleteLayer(Uint16 layer);
		LayerObjectTree& getLayers();
		Uint16 getCurrentLayer();
		void setCurrentLayer(Uint16);
		//成功返回true反则false
		bool getLayerFromID(GObjectID id, Uint16& layer);
		void setObjectToLayer(GObjectID id, Uint16 layer = 0);
		void removeObjectFromLayer(GObjectID id);
		b2World& getWorld() { return world; }
		template<typename Archive>
		void serialize(Archive& archive)
		{
			float gravity[2] = { world.GetGravity().x,world.GetGravity().y };
			bool subStep = world.GetSubStepping();
			bool warmStart = world.GetWarmStarting();
			bool allowSleep = world.GetAllowSleeping();
			archive(
				cereal::make_nvp("level_url", m_level_url),
				cereal::make_nvp("level_name", m_level_name),
				cereal::make_nvp("window_size", window_size),
				cereal::make_nvp("tex_size", tex_size),
				cereal::make_nvp("render_rect_size", render_rect_size),
				cereal::make_nvp("objects", m_objects),
				cereal::make_nvp("layers", m_layers),
				cereal::make_nvp("gravity_x", gravity[0]),
				cereal::make_nvp("gravity_y", gravity[1]),
				cereal::make_nvp("subStep", subStep),
				cereal::make_nvp("warmStart", warmStart),
				cereal::make_nvp("allowSleep", allowSleep)
			);
			if (typeid(archive) == typeid(cereal::JSONInputArchive)
				|| typeid(archive) == typeid(cereal::PortableBinaryInputArchive)) {
				this->setTargetTextureSize(tex_size);
				int w, h;
				SDL_GetWindowSize(RuntimeGlobalContext::SDL_CONTEXT.window, &w, &h);
				if(w != int(window_size[0]) || h != int(window_size[1]))
					setWindowSize(window_size);
				world.SetGravity(b2Vec2(gravity[0], gravity[1]));
				world.SetWarmStarting(warmStart);
				world.SetSubStepping(subStep);
				world.SetAllowSleeping(allowSleep);
				for (auto& obj : m_objects) {
					obj.second->setLevel(shared_from_this());
				}
			}
		}

		void initTextureSize() {
			this->setTargetTextureSize(tex_size);
		}
		void initWindowSize();
		void setWindowSize(std::array<float, 2> size) {
			size[0] = std::clamp(size[0], 640.0f, 5000.0f);
			size[1] = std::clamp(size[1], 480.0f, 5000.0f);
			window_size = size;
			initWindowSize();
		}
		std::array<float, 2> getWindowSize();
		std::shared_ptr<Level> getSharedPtr()
		{
			return shared_from_this();
		}
		std::array<float, 2> getTargetTextureSize()const;
		void setTargetTextureSize(std::array<float, 2>);
	private:
		LevelObjectMap m_objects;
		LayerObjectTree m_layers;
		std::string m_level_url;
		std::string m_level_name;
		Uint16 m_current_layer;
		b2World world;
		std::array<float, 2> window_size;
		std::array<float, 2> tex_size;
		std::array<float, 4> render_rect_size;
		RTTR_ENABLE()
			RTTR_REGISTRATION_FRIEND
	};
}