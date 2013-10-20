#include "EntityQuery.hpp"
#include "EntitySystem.hpp"
#include "math/vec.hpp"
#include <iostream>
#include "TextureManager.hpp"
#include "memory/ObjectPool.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include "video.hpp"
#include "gl/gl_1_5.h"
#include "gl/gl_assert.hpp"
#include "render/SpriteBuffer.hpp"
#include "memory/TypedDynamicPool.hpp"

using namespace yks;

EntityWorld world;

struct Position  {
	static const ComponentTypeId component_id = 0;

	vec2i position;

	Position(int x, int y)
		: position{{x, y}}
	{}
};
ObjectPool<Position> positionPool;

struct Velocity {
	static const ComponentTypeId component_id = 1;

	vec2i velocity;

	Velocity(int x, int y)
		: velocity{{x, y}}
	{}
};
ObjectPool<Velocity> velocityPool;

struct CircleRenderer {
	static const ComponentTypeId component_id = 2;

	int radius;

	CircleRenderer(int radius)
		: radius(radius)
	{}
};
ObjectPool<CircleRenderer> circleRendererPool;

TextureManager texture_manager;

int main(int argc, char *argv[]) {
	world.addComponentType(Position::component_id, "Position");
	world.addComponentType(Velocity::component_id, "Velocity");
	world.addComponentType(CircleRenderer::component_id, "CircleRenderer");

	Handle e0 = world.createEntity("pos");
	Handle e1 = world.createEntity("pos_circle");
	Handle e2 = world.createEntity("pos_vel");
	Handle e3 = world.createEntity("pos_vel_circle1");
	Handle e4 = world.createEntity("pos_vel_circle2");

	world.addComponentToEntity(positionPool, e0, 0, 0);

	world.addComponentToEntity(positionPool, e1, 0, 0);
	world.addComponentToEntity(circleRendererPool, e1, 4);

	world.addComponentToEntity(positionPool, e2, 0, 0);
	world.addComponentToEntity(velocityPool, e2, 2, 1);

	world.addComponentToEntity(positionPool, e3, 0, 0);
	world.addComponentToEntity(velocityPool, e3, 1, 2);
	world.addComponentToEntity(circleRendererPool, e3, 4);

	world.addComponentToEntity(positionPool, e4, 0, 0);
	world.addComponentToEntity(velocityPool, e4, 2, 2);
	world.addComponentToEntity(circleRendererPool, e4, 8);

	Window window;
	if (!window.open(640, 480)) {
		return 1;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	GLfloat view_matrix[16] = {
		2.0f / window.width, 0, 0, -1,
		0, -2.0f / window.height, 0, 1,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	glLoadTransposeMatrixf(view_matrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	YKS_CHECK_GL;

	glEnable(GL_TEXTURE_2D);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	Handle tex = texture_manager.loadTexture("test", "data/test.png");

	SpriteBufferIndices spr_indices;
	SpriteBuffer main_buffer;
	main_buffer.texture_size = vec2i{{64, 64}};

	Sprite spr;

	for (;;) {
		main_buffer.clear();

		glClear(GL_COLOR_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, texture_manager.texture_pool[tex]->api_handle);

		for (auto i : query(world, Position::component_id, Velocity::component_id)) {
			positionPool[i[0]]->position += velocityPool[i[1]]->velocity;
		}

		for (auto i : query(world, Position::component_id, CircleRenderer::component_id)) {
			spr.pos = positionPool[i[0]]->position;
			spr.img = IntRect{0, 0, 64, 64};
			main_buffer.append(spr);
		}

		main_buffer.draw(spr_indices);

		window.flip();

		SDL_Event ev;
		SDL_WaitEvent(&ev);
		if (ev.type == SDL_QUIT) {
			break;
		}
	}

	window.close();
	SDL_Quit();

	return 0;
}
