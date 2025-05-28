#pragma once
#include "../../core/global/global.hpp"

namespace GearX {
	extern Mix_Music* loadMusic(const std::string& path);
	extern Mix_Chunk* loadChunk(const std::string& path);
}