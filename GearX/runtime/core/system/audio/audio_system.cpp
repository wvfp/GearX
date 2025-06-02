#include "audio_system.hpp"
#include "../../global/global.hpp"
namespace GearX {
    static void CleanUp(Mix_Music* music)
    {
        if (Mix_PlayingMusic()) {
            Mix_FadeOutMusic(1500);
            SDL_Delay(1500);
        }
        if (music) {
            Mix_FreeMusic(music);
            music = NULL;
        }
		Mix_CloseAudio();
    }

    bool AudioSystem::isStarting = false;
    Asset AudioSystem::music = Asset();
    constexpr Uint8 Playing = 1;
    constexpr Uint8 Paused = 2;
    constexpr Uint8 Finished = 3;
    std::map<int, Asset> AudioSystem::channels = std::map<int, Asset>();
	std::map<int,Uint8> AudioSystem::channel_state = std::map<int,Uint8>();
    AudioSystem::AudioSystem() {
     
    };
    AudioSystem::~AudioSystem() {
        ////“Ù∆µÕÀ≥ˆ
        //auto assets = RuntimeGlobalContext::assetManager.getAllAsset();

        //for (auto& asset : assets) {
        //    if (asset.second.type == AssetType::Music) {
        //        CleanUp(static_cast<Mix_Music*>(asset.second.data));
        //    }else if (asset.second.type == AssetType::Chunk) {
        //        Mix_FreeChunk(static_cast<Mix_Chunk*>(asset.second.data));
        //        asset.second.data = nullptr;
        //    }
        //}
        //Mix_CloseAudio();
        //Mix_Quit();
        //SDL_CloseAudioDevice(0);
    };
    void AudioSystem::tick(float deltaTime) { 
    }
    void AudioSystem::Start(){
        if (!isStarting) {
            //“Ù∆µ≥ı ºªØ
            Mix_Init(MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_OPUS);
            SDL_AudioSpec spec;
            SDL_zero(spec);
            /* Initialize variables */
            spec.freq = MIX_DEFAULT_FREQUENCY;
            spec.format = MIX_DEFAULT_FORMAT;
            spec.channels = MIX_DEFAULT_CHANNELS;
            spec.format = SDL_AUDIO_F32;
            SDL_OpenAudioDevice(0, &spec);
            Mix_OpenAudio(0, &spec);
            // ◊¢≤·“Ù∆µΩ· ¯ªÿµ˜
            Mix_ChannelFinished(ChannelFinised);
			isStarting = true;
        }
    }
    void AudioSystem::Destory(){
        if (isStarting) {
			Mix_HaltChannel(-1);
            Mix_HaltMusic();
            CleanUp(static_cast<Mix_Music*>(music.data));
            isStarting = false;
        }
    }
    void AudioSystem::loadMusicFromPath(const std::string& path) {
        music = RuntimeGlobalContext::assetManager.loadAssetMusic(path);
    }
    void AudioSystem::unloadMusic() {
        music.data = nullptr;
    }

    void AudioSystem::PlayMusic() {
        if (music.data && ((Mix_PlayingMusic() == 0))) {
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
            int c = Mix_PlayChannel(-1, static_cast<Mix_Chunk*>(chunk.data), 0);
            channels[c] = chunk;
            channel_state[c] = Playing;
        }
    }

    void AudioSystem::StopChunk(std::string path) {
        auto chunk = RuntimeGlobalContext::assetManager.loadAssetChunk(path);
        if (chunk.data) {
            int c = getChannel(path);
            if (c != -1) {
                Mix_HaltChannel(c);
                channel_state[c] = Finished;
            }
        }
    }

    void AudioSystem::PauseChunk(std::string path) {
        for (auto it = channels.begin(); it != channels.end(); ++it) {
            if (it->second.asset_url == path) {
                Mix_Pause(it->first);
                channel_state[it->first] = Paused;
                return;
            }
        }
    }

    void AudioSystem::ResumeChunk(std::string path) {
		for (auto it = channels.begin(); it != channels.end(); ++it) {
			if (it->second.asset_url == path) {
				Mix_Resume(it->first);
                channel_state[it->first] = Playing;
				return;
            }
        }

    }

    int AudioSystem::getChunkState(const std::string& path){
        for (auto it = channels.begin(); it != channels.end(); ++it) {
            if (it->second.asset_url == path) {
                return channel_state[it->first];
            }
        }
        return Finished;
    }

    int AudioSystem::getChannel(const std::string& path){
        for (auto it = channels.begin(); it != channels.end(); ++it) {
            if (it->second.asset_url == path) {
                return it->first;
            }
        }
        return -1;
    }

    void AudioSystem::ChannelFinised(int channel){
        channel_state[channel] = Finished;
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
        table["getChunkState"] = &AudioSystem::getChunkState;
        table["getChannel"] = &AudioSystem::getChannel;
        lua["Audio"] = table;
    }

}
