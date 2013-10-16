#pragma once

#include <SDL2/SDL_video.h>

struct Window {
	SDL_Window* window = nullptr;
	SDL_GLContext gl_context = nullptr;
	int width = -1;
	int height = -1;

	bool open(int width, int height);
	void close();
	~Window();

	void flip();
};

