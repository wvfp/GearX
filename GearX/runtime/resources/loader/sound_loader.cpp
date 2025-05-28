#include "sound_loader.hpp"

Mix_Music* GearX::loadMusic(const std::string& path) {
	return Mix_LoadMUS(path.c_str());
}

Mix_Chunk* GearX::loadChunk(const std::string& path) {
	return Mix_LoadWAV(path.c_str());
}