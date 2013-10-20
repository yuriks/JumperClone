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

	vec2 position;

	Position(vec2 position)
		: position(position)
	{}
};
ObjectPool<Position> positionPool;

struct Velocity {
	static const ComponentTypeId component_id = 1;

	vec2 velocity;

	Velocity(vec2 velocity)
		: velocity(velocity)
	{}
};
ObjectPool<Velocity> velocityPool;

struct SpriteRenderer {
	static const ComponentTypeId component_id = 2;

	int layer;
	IntRect img_rect;

	SpriteRenderer(int layer, IntRect img_rect)
		: layer(layer), img_rect(img_rect)
	{}
};
ObjectPool<SpriteRenderer> spriteRendererPool;

struct Gravity {
	static const ComponentTypeId component_id = 3;

	vec2 acceleration;

	Gravity(vec2 acceleration)
		: acceleration(acceleration)
	{}
};
ObjectPool<Gravity> gravityPool;

TextureManager texture_manager;

int main(int argc, char *argv[]) {
	world.addComponentType(Position::component_id, "Position");
	world.addComponentType(Velocity::component_id, "Velocity");
	world.addComponentType(SpriteRenderer::component_id, "SpriteRenderer");
	world.addComponentType(Gravity::component_id, "Gravity");

	Handle e0 = world.createEntity("pos");
	Handle e1 = world.createEntity("pos_circle");
	Handle e2 = world.createEntity("pos_vel");
	Handle e3 = world.createEntity("pos_vel_circle1");
	Handle e4 = world.createEntity("pos_vel_circle2");

	world.addComponentToEntity(positionPool, e0, vec2{{0, 0}});

	world.addComponentToEntity(positionPool, e1, vec2{{0, 0}});
	world.addComponentToEntity(spriteRendererPool, e1, 0, IntRect{32, 0, 16, 16});

	world.addComponentToEntity(positionPool, e2, vec2{{0, 0}});
	world.addComponentToEntity(velocityPool, e2, vec2{{2, 1}});

	world.addComponentToEntity(positionPool, e3, vec2{{0, 0}});
	world.addComponentToEntity(velocityPool, e3, vec2{{1, 0}});
	world.addComponentToEntity(gravityPool, e3, vec2{{0, 0.03}});
	world.addComponentToEntity(spriteRendererPool, e3, 0, IntRect{32, 0, 16, 16});

	world.addComponentToEntity(positionPool, e4, vec2{{0, 0}});
	world.addComponentToEntity(velocityPool, e4, vec2{{2, 2}});
	world.addComponentToEntity(spriteRendererPool, e4, 0, IntRect{32, 0, 16, 16});

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

	Handle tex = texture_manager.loadTexture("test", "data/sprites.png");

	SpriteBufferIndices spr_indices;
	SpriteBuffer main_buffer;
	main_buffer.texture_size = texture_manager[tex]->size;

	Sprite spr;

	for (;;) {
		main_buffer.clear();

		glClear(GL_COLOR_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, texture_manager[tex]->api_handle);

		query_for_each(world, std::tie(velocityPool, gravityPool), [&](Velocity& vel, const Gravity& gravity) {
			vel.velocity += gravity.acceleration;
		});

		query_for_each(world, std::tie(positionPool, velocityPool), [&](Position& pos, const Velocity& vel) {
			pos.position += vel.velocity;
		});

		query_for_each(world, std::tie(positionPool, spriteRendererPool), [&](const Position& pos, const SpriteRenderer& renderer) {
			spr.pos = pos.position.typecast<int>();
			spr.img = renderer.img_rect;
			main_buffer.append(spr);
		});

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
