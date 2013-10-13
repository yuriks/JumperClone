#include "EntitySystem.hpp"
#include "math/vec.hpp"
#include <iostream>

using namespace yks;

EntityWorld world;

struct Position  {
	static const ComponentTypeId component_id = 0;

	vec2i position;
};

struct Velocity {
	static const ComponentTypeId component_id = 1;

	vec2i velocity;
};

struct DrawCircle {
	static const ComponentTypeId component_id = 2;

	int radius;
};

int main() {
	world.addComponentType(Position::component_id, "position");
	world.addComponentType(Velocity::component_id, "velocity");
	world.addComponentType(DrawCircle::component_id, "draw_circle");

	world.createEntity("0");
	world.createEntity("1");
	world.createEntity("2");
	world.createEntity("3");
	world.createEntity("4");

	world.addComponentToEntity(0, Position::component_id);

	world.addComponentToEntity(1, Position::component_id);
	world.addComponentToEntity(1, DrawCircle::component_id);

	world.addComponentToEntity(2, Position::component_id);
	world.addComponentToEntity(2, Velocity::component_id);

	world.addComponentToEntity(3, Position::component_id);
	world.addComponentToEntity(3, Velocity::component_id);
	world.addComponentToEntity(3, DrawCircle::component_id);

	world.addComponentToEntity(4, Position::component_id);
	world.addComponentToEntity(4, Velocity::component_id);
	world.addComponentToEntity(4, DrawCircle::component_id);

	for (auto i : query(world, Position::component_id, Velocity::component_id)) {
		std::cout << i[0] << " " << i[1] << std::endl;
	}

	std::cout << "Done!" << std::endl;
}
