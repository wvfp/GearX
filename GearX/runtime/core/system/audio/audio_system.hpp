#pragma once
#include "../../../resources/asset_manager/asset.hpp"
#include "../system.hpp"
namespace GearX {
    class AudioSystem : public GearX::System {
    public:
        AudioSystem();
        ~AudioSystem();
        void tick(float deltaTime);
        static void Start();
        static void Destory();
        //Music
        static void loadMusicFromPath(const std::string& path);
		static void unloadMusic();
		static void PlayMusic();
		static void StopMusic();
		static void PauseMusic();
		static void ResumeMusic();
	   //Chunck
	   static void PlayChunk(std::string path);
	   static void StopChunk(std::string path);
	   static void PauseChunk(std::string path);
	   static void ResumeChunk(std::string path);
	   static int getChunkState(const std::string& path);
	   static int getChannel(const std::string& path);
	   static void ChannelFinised(int channel);
        //setter
        //Music
	   static  void SetMusicVolume(float volume);
	   static  void SetMusicPosition(float position);
	    //Chunck
       static  void SetChunkVolume(const std::string& path,float volume);
        //getter
        //Music
	   static  float GetMusicVolume();
	   static  float GetMusicLoopStartTime();
	   static  float GetMusicLoopEndTime();
	   static  float GetMusicLoopLengthTime();
	   static  float GetMusicPosition();
        //Chunck
	   static  float GetChunkVolume(const std::string& path);
	   static  void RegisterToLua(sol::state& lua);
    private:
        static Asset music;
        static std::map<int,Asset> channels;
		static std::map<int,Uint8> channel_state;
        static bool isStarting;
    };
}
