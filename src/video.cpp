#include "video.hpp"
#include "gl/gl_1_5.h"
#include <SDL2/SDL.h>
#include <iostream>

bool Window::open(int width_, int height_) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "Couldn't init SDL: " << SDL_GetError() << '\n';
		return false;
	}

	window = SDL_CreateWindow("Foobar", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width_, height_, SDL_WINDOW_OPENGL);
	if (window == nullptr) {
		std::cerr << "Couldn't open window: " << SDL_GetError() << '\n';
		close();
		return false;
	}
	width = width_;
	height = height_;

	gl_context = SDL_GL_CreateContext(window);
	if (gl_context == nullptr) {
		std::cerr << "Couldn't create OpenGL context: " << SDL_GetError() << '\n';
		close();
		return false;
	}
	SDL_GL_MakeCurrent(window, gl_context);

	if (ogl_LoadFunctions() != ogl_LOAD_SUCCEEDED) {
		std::cerr << "Couldn't init OpenGL functions.\n";
		close();
		return false;
	}

	if (ogl_IsVersionGEQ(1, 5)) {
		std::cerr << "OpenGL 1.5 not supported.\n";
		close();
		return false;
	}
	
	glViewport(0, 0, width, height);

	return true;
}

void Window::close() {
	if (gl_context != nullptr) {
		SDL_GL_DeleteContext(gl_context);
		gl_context = nullptr;
	}
	if (window != nullptr) {
		SDL_DestroyWindow(window);
		window = nullptr;
		width = height = -1;
	}
}

Window::~Window() {
	close();
}

void Window::flip() {
	SDL_GL_SwapWindow(window);
}
