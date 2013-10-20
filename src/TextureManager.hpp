#pragma once
#include "Handle.hpp"
#include "memory/ObjectPool.hpp"
#include "math/vec.hpp"
#include <string>

struct Texture {
	uint32_t api_handle;
	yks::vec2i size;
	std::string name;
};

struct TextureManager {
	struct LoadOptions {
		bool premultiplied_alpha = true;
	};

	yks::ObjectPool<Texture> texture_pool;

	yks::Handle loadTexture(const std::string& name, const std::string& filename, LoadOptions options = LoadOptions());
	void deleteTexture(yks::Handle h);
};
