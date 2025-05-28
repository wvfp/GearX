#include "audio_system.hpp"
#include "../../global/global.hpp"
namespace GearX {
    AudioSystem::AudioSystem() {
    };
    AudioSystem::~AudioSystem() {
    };
    void AudioSystem::tick(float deltaTime) {
    
    }
    void AudioSystem::loadMusicFromPath(const std::string& path) {
        music = RuntimeGlobalContext::assetManager.loadAssetMusic(path);
    }
    void AudioSystem::unloadMusic() {
        music.data = nullptr;
    }

    void AudioSystem::PlayMusic() {
        if (music.data && ((Mix_PlayingMusic() == 0) || isMusicPlaying)) {
            Mix_PlayMusic(static_cast<Mix_Music*>(music.data), -1);
        }
	}

    void AudioSystem::StopMusic() {
        if (Mix_PlayingMusic() == 1) {
            Mix_HaltMusic();
        }
    }

    void AudioSystem::PauseMusic() {
        if (Mix_PlayingMusic() == 1) {
            Mix_PauseMusic();
        }
    }

    void AudioSystem::ResumeMusic() {
        if (Mix_PausedMusic() == 1) {
            Mix_ResumeMusic();
        }
    }

    void AudioSystem::PlayChunk(std::string path) {
        auto chunk = RuntimeGlobalContext::assetManager.loadAssetChunk(path);
        if (chunk.data) {
            channels[Mix_PlayChannel(-1, static_cast<Mix_Chunk*>(chunk.data), 0)] = chunk;
        }
    }

    void AudioSystem::StopChunk(std::string path) {
        auto chunk = RuntimeGlobalContext::assetManager.loadAssetChunk(path);
        if (chunk.data) {
            Mix_HaltChannel(-1);
        }
    }

    void AudioSystem::PauseChunk(std::string path) {
        for (auto it = channels.begin(); it != channels.end(); ++it) {
            if (it->second.asset_url == path) {
                Mix_Pause(it->first);
                return;
            }
        }
    }

    void AudioSystem::ResumeChunk(std::string path) {
		for (auto it = channels.begin(); it != channels.end(); ++it) {
			if (it->second.asset_url == path) {
				Mix_Resume(it->first);
				return;
            }
        }

    }

    void AudioSystem::SetMusicVolume(float volume) {
        volume = SDL_clamp(volume, 0.0f, 1.0f);
        volume = volume * MIX_MAX_VOLUME;
        Mix_Volume(-1, static_cast<int>(volume));
    }


    void AudioSystem::SetMusicPosition(float position) {
        if (music.data) {
            Mix_SetMusicPosition(position);
        }
    }

    void AudioSystem::SetChunkVolume(const std::string& path, float volume) {
        for (auto it = channels.begin(); it != channels.end(); ++it) { 
            if (it->second.asset_url == path) {
                Mix_Volume(it->first, static_cast<int>(volume * MIX_MAX_VOLUME));
                return;
            }
        }
    }

    float AudioSystem::GetMusicVolume() {
        if (music.data) {
            return Mix_VolumeMusic(-1) / static_cast<float>(MIX_MAX_VOLUME);
        }
        return 0.0f;
    }

    float AudioSystem::GetMusicLoopStartTime() {
        if (music.data) {
            return Mix_GetMusicLoopStartTime(static_cast<Mix_Music*>(music.data));
        }
        return 0.0f;
    }

    float AudioSystem::GetMusicLoopEndTime() {
        if (music.data) {
            return Mix_GetMusicLoopEndTime(static_cast<Mix_Music*>(music.data));
        }
        return 0.0f;
    }
    float AudioSystem::GetMusicLoopLengthTime() {
        if (music.data) {
            return Mix_GetMusicLoopLengthTime(static_cast<Mix_Music*>(music.data));
        }
        return 0.0f;
    }
    float AudioSystem::GetMusicPosition() {
        if(music.data) {
            return Mix_GetMusicPosition(static_cast<Mix_Music*>(music.data));
        }
        return 0.0f;
    }

    float AudioSystem::GetChunkVolume(const std::string& path) {
        for (auto it = channels.begin(); it != channels.end(); ++it) {
            if (it->second.asset_url == path) {
                return Mix_Volume(it->first, -1) / static_cast<float>(MIX_MAX_VOLUME);
            }
        }
        return 0.0f;
    }

    void AudioSystem::RegisterToLua(sol::state& lua){
        auto& table = lua.create_table();
        table["getMusicVolume"] = &AudioSystem::GetMusicVolume;
        table["getMusicLoopStartTime"] = &AudioSystem::GetMusicLoopStartTime;
        table["getMusicLoopEndTime"] = &AudioSystem::GetMusicLoopEndTime;
        table["getMusicLoopLengthTime"] = &AudioSystem::GetMusicLoopLengthTime;
        table["getMusicPosition"] = &AudioSystem::GetMusicPosition;
        table["getChunkVolume"] = &AudioSystem::GetChunkVolume;
        table["setMusicVolume"] = &AudioSystem::SetMusicVolume;
        table["setMusicPosition"] = &AudioSystem::SetMusicPosition;
        table["setChunkVolume"] = &AudioSystem::SetChunkVolume;
        table["playMusic"] = &AudioSystem::PlayMusic;
        table["stopMusic"] = &AudioSystem::StopMusic;
        table["pauseMusic"] = &AudioSystem::PauseMusic;
        table["resumeMusic"] = &AudioSystem::ResumeMusic;
        table["playChunk"] = &AudioSystem::PlayChunk;
        table["stopChunk"] = &AudioSystem::StopChunk;
        table["pauseChunk"] = &AudioSystem::PauseChunk;
        table["resumeChunk"] = &AudioSystem::ResumeChunk;
        table["loadMusicFromPath"] = &AudioSystem::loadMusicFromPath;
        table["unloadMusic"] = &AudioSystem::unloadMusic;
        lua["Audio"] = table;
    }

}
