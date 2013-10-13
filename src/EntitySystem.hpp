#pragma once
#include "SortedVector.hpp"
#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

typedef uint32_t ComponentTypeId;
static const ComponentTypeId invalid_component_type = ~0;
struct ComponentType {
	std::string name;

	ComponentType() {}
	ComponentType(const std::string& name)
		: name(name)
	{}
};

typedef uint32_t EntityId;
static const EntityId invalid_entity = ~0;
struct Entity {
	std::string name;

	Entity() {}
	Entity(const std::string& name)
		: name(name)
	{}
};

typedef uint32_t ComponentId;
static const ComponentId invalid_component = ~0;

struct EntityWorld {
	typedef SortedVector<std::tuple<EntityId, ComponentId>> EntityComponentMap;
	typedef SortedVector<std::tuple<ComponentTypeId, ComponentId>> ComponentEntityMap;

	std::vector<ComponentType> component_types;
	std::vector<Entity> entities;
	std::vector<ComponentId> component_ids;
	std::vector<EntityComponentMap> entities_by_component;
	std::vector<ComponentEntityMap> components_by_entity;

	void addComponentType(ComponentTypeId id, const std::string& name);
	EntityId createEntity(const std::string& name);
	ComponentId addComponentToEntity(EntityId entity, ComponentTypeId type);
	void removeComponentFromEntity(EntityId entity, ComponentTypeId type);
};
