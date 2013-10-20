#include "TextureManager.hpp"
#include "gl/gl_1_5.h"
#include "stb_image.h"
#include <memory>

static uint32_t loadGLTexture(int width, int height, const uint8_t* data) {
	static_assert(sizeof(uint32_t) == sizeof(GLuint), "Types don't have the same range!");

	GLuint h;
	glGenTextures(1, &h);

	glBindTexture(GL_TEXTURE_2D, h);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	return h;
}

static void freeGLTexture(uint32_t h) {
	glDeleteTextures(1, &h);
}

yks::Handle TextureManager::loadTexture(const std::string& name, const std::string& filename, LoadOptions options) {
	int width, height, comp;
	auto data = std::unique_ptr<unsigned char[], void(*)(void*)>(
		stbi_load(filename.c_str(), &width, &height, &comp, 4), &stbi_image_free);
	if (data == nullptr)
		return yks::Handle();

	if (options.premultiplied_alpha) {
		size_t size = width * height * 4;

		for (unsigned int i = 0; i < size; i += 4) {
			unsigned int a = data[i + 3];
			data[i+0] = data[i+0] * a / 255;
			data[i+1] = data[i+1] * a / 255;
			data[i+2] = data[i+2] * a / 255;
		}
	}

	uint32_t api_handle = loadGLTexture(width, height, data.get());

	Texture tex;
	tex.api_handle = api_handle;
	tex.size = yks::vec2i{{width, height}};
	tex.name = name;

	yks::Handle h = texture_pool.emplace(tex);
	return h;
}

void TextureManager::deleteTexture(yks::Handle h) {
	Texture* tex = texture_pool[h];
	freeGLTexture(tex->api_handle);
	texture_pool.remove(h);
}
