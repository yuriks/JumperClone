#include "EntitySystem.hpp"
#include <cassert>

bool EntityWorld::typeExists(ComponentTypeId type) {
	return !component_types[type].name.empty();
}

void EntityWorld::addComponentType(ComponentTypeId id, const std::string& name) {
	if (component_types.size() < id + 1) {
		component_types.resize(id + 1);
		components_by_component_type.resize(id + 1);
	}

	assert(!typeExists(id));
	component_types[id] = ComponentType(name);
	assert(component_types.size() == components_by_component_type.size());
}

EntityId EntityWorld::createEntity(const std::string& name) {
	return entities.emplace(name);
}

void EntityWorld::addComponentToEntity(EntityId entity, ComponentTypeId type, ComponentHandle handle) {
	assert(typeExists(type));

	entities[entity]->components.insert(std::make_tuple(type, handle));
	components_by_component_type[type].insert(std::make_tuple(entity, handle));
}

void EntityWorld::removeComponentFromEntity(EntityId entity, ComponentTypeId type) {
	assert(typeExists(type));

	entities[entity]->components.remove(type);
	components_by_component_type[type].remove(entity);
}
