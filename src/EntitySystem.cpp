#include "EntitySystem.hpp"
#include <cassert>

void EntityWorld::addComponentType(ComponentTypeId id, const std::string& name) {
	if (component_types.size() < id + 1) {
		component_types.resize(id + 1);
		component_ids.resize(id + 1, invalid_component);
		entities_by_component.resize(id + 1);
	} else {
		assert(component_ids[id] == invalid_component);
	}
	component_types[id] = ComponentType(name);
	component_ids[id] = 0;
	assert(component_types.size() == component_ids.size() && component_types.size() == entities_by_component.size());
}

EntityId EntityWorld::createEntity(const std::string& name) {
	entities.emplace_back(name);
	components_by_entity.emplace_back();
	return entities.size() - 1;
}

ComponentId EntityWorld::addComponentToEntity(EntityId entity, ComponentTypeId type) {
	assert(type < component_ids.size() && component_ids[type] != invalid_component);
	ComponentId id = component_ids[type]++;

	components_by_entity[entity].insert(std::make_tuple(type, id));
	entities_by_component[type].insert(std::make_tuple(entity, id));
	return id;
}

void EntityWorld::removeComponentFromEntity(EntityId entity, ComponentTypeId type) {
	assert(type < component_ids.size() && component_ids[type] != invalid_component);

	components_by_entity[entity].remove(type);
	entities_by_component[type].remove(entity);
}