#pragma once
#include "../../../resources/asset_manager/asset.hpp"
#include "../system.hpp"
namespace GearX {
    class AudioSystem : public GearX::System {
    public:
        AudioSystem();
        ~AudioSystem();
        void tick(float deltaTime);
        //Music
        void loadMusicFromPath(const std::string& path);
        void unloadMusic();
        void PlayMusic();
        void StopMusic();
        void PauseMusic();
        void ResumeMusic();
        //Chunck
        void PlayChunk(std::string path);
        void StopChunk(std::string path);
        void PauseChunk(std::string path);
        void ResumeChunk(std::string path);
        //setter
        //Music
        void SetMusicVolume(float volume);
        void SetMusicPosition(float position);
	    //Chunck
        void SetChunkVolume(const std::string& path,float volume);
        //getter
        //Music
        float GetMusicVolume();
        float GetMusicLoopStartTime();
        float GetMusicLoopEndTime();
        float GetMusicLoopLengthTime();
        float GetMusicPosition();
        //Chunck
        float GetChunkVolume(const std::string& path);
        void RegisterToLua(sol::state& lua);
    private:
        Asset music;
        std::map<int,Asset> channels;
        bool isMusicPlaying = false;
    };
}
